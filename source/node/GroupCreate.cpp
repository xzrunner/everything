#include "sop/node/GroupCreate.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/EvalContext.h"
#include "sop/Evaluator.h"

#include <SM_Calc.h>
#include <model/BrushModel.h>
#include <polymesh3/Geometry.h>
#include <halfedge/Polyhedron.h>

namespace sop
{
namespace node
{

void GroupCreate::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, IDX_SOURCE_OBJ);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    auto& group_mgr = m_geo_impl->GetGroup();

    auto group = std::make_shared<Group>();

    auto& props = m_props.GetProps();
    assert(props[NAME].Val().type == VarType::String);
    group->SetName(static_cast<const char*>(props[NAME].Val().p));
    assert(props[TYPE].Val().type == VarType::Int);
    auto group_type = static_cast<GroupType>(props[TYPE].Val().i);
    group->SetType(group_type);

    assert(props[BASE_ON].Val().type == VarType::Bool);
    assert(props[BOUNDING_ON].Val().type == VarType::Bool);
    assert(props[NORMAL_ON].Val().type == VarType::Bool);
    if (props[BASE_ON].Val().b)
    {
        assert(props[BASE_GROUP].Val().type == VarType::String);
        std::string expr = static_cast<const char*>(props[BASE_GROUP].Val().p);
        if (expr.empty()) {
            SelectAll(*group);
        } else {
            group->SetItems(NodeHelper::SelectGeoByExpr(group_type, eval, *this, expr));
        }
    }
    else if (props[BOUNDING_ON].Val().b)
    {
        auto b_geo = NodeHelper::GetInputGeo(*this, IDX_BOUNDING_OBJ);
        if (b_geo) {
            SelectByBoundings(*group, b_geo);
        }
    }
    else if (props[NORMAL_ON].Val().b)
    {
        SelectByNormals(*group);
    }

    group_mgr.Add(group, GetMergeOP());
    m_props.SetValue(NAME, Variable(group->GetName()));
}

void GroupCreate::SetGroupName(const std::string& name)
{
    if (m_props.SetValue(NAME, Variable(name))) {
        SetDirty(true);
    }
}

void GroupCreate::SetGroupType(GroupType type)
{
    if (m_props.SetValue(TYPE, Variable(static_cast<int>(type)))) {
        SetDirty(true);
    }
}

void GroupCreate::SetGroupMerge(GroupMerge merge)
{
    std::string str = "replace";
    switch (merge)
    {
    case GroupMerge::Replace:
        str = "replace";
        break;
    case GroupMerge::Union:
        str = "union";
        break;
    default:
        assert(0);
    }

    if (m_props.SetValue(MERGE, Variable(str))) {
        SetDirty(true);
    }
}

void GroupCreate::EnableBaseGroup(const std::string& expr)
{
    auto& props = m_props.GetProps();
    assert(props[BASE_ON].Val().type == VarType::Bool 
        && props[BASE_GROUP].Val().type == VarType::String);
    if (props[BASE_ON].Val().b &&
        static_cast<const char*>(props[BASE_GROUP].Val().p) == expr) {
        return;
    }

    m_props.SetValue(BASE_ON,    Variable(true));
    m_props.SetValue(BASE_GROUP, Variable(expr));

    SetDirty(true);
}

void GroupCreate::DisableBaseGroup()
{
    if (m_props.SetValue(BASE_ON, Variable(false))) {
        SetDirty(true);
    }
}

void GroupCreate::EnableKeepInBounding()
{
    if (m_props.SetValue(BOUNDING_ON, Variable(true))) {
        SetDirty(true);
    }
}

void GroupCreate::DisableKeepInBounding()
{
    if (m_props.SetValue(BOUNDING_ON, Variable(false))) {
        SetDirty(true);
    }
}

void GroupCreate::EnableKeepByNormals(const sm::vec3& direction, float spread_angle)
{
    bool dir_dirty = false;
    if (m_props.SetValue(DIR, Variable(direction))) {
        dir_dirty = true;
    }

    auto& props = m_props.GetProps();
    assert(props[NORMAL_ON].Val().type == VarType::Bool
        && props[ANGLE].Val().type == VarType::Float);
    if (props[NORMAL_ON].Val().b &&
        !dir_dirty &&
        props[ANGLE].Val().f == spread_angle) {
        return;
    }

    m_props.SetValue(NORMAL_ON, Variable(true));
    m_props.SetValue(ANGLE,     Variable(spread_angle));

    SetDirty(true);
}

void GroupCreate::DisableKeepByNormals()
{
    if (m_props.SetValue(NORMAL_ON, Variable(false))) {
        SetDirty(true);
    }
}

void GroupCreate::InitProps()
{
    m_props.Assign(NAME,  PropNames[NAME],  Variable(std::string()));
    m_props.Assign(TYPE,  PropNames[TYPE],  Variable(static_cast<int>(GroupType::Primitives)));
    m_props.Assign(MERGE, PropNames[MERGE], Variable(std::string("replace")));

    m_props.Assign(BASE_ON,    PropNames[BASE_ON],    Variable(false));
    m_props.Assign(BASE_GROUP, PropNames[BASE_GROUP], Variable(std::string()));

    m_props.Assign(BOUNDING_ON, PropNames[BOUNDING_ON], Variable(false));

    m_props.Assign(NORMAL_ON, PropNames[NORMAL_ON], Variable(false));
    m_props.Assign(DIR,       PropNames[DIR],       Variable(sm::vec3(0.0f, 0.0f, 1.0f)));
    m_props.Assign(ANGLE,     PropNames[ANGLE],     Variable(180.0f));
}

void GroupCreate::SelectByNormals(Group& group)
{
    auto& props = m_props.GetProps();
    assert(props[TYPE].Val().type == VarType::Int);
    switch (static_cast<GroupType>(props[TYPE].Val().i))
    {
    case GroupType::Points:
        break;
    case GroupType::Vertices:
        break;
    case GroupType::Edges:
        break;
    case GroupType::Primitives:
    {
        assert(props[DIR].Val().type == VarType::Float3);
        auto dir = static_cast<const sm::vec3*>(props[DIR].Val().p);

        auto& prims = m_geo_impl->GetAttr().GetPrimtives();
        for (size_t i = 0, n = prims.size(); i < n; ++i)
        {
            auto normal = prims[i]->CalcNormal();
            auto angle = sm::get_angle(sm::vec3(0, 0, 0), *dir, normal);
            assert(props[ANGLE].Val().type == VarType::Float);
            if (angle <= props[ANGLE].Val().f * SM_DEG_TO_RAD) {
                group.Add(i);
            }
        }
    }
        break;
    default:
        assert(0);
    }
}

void GroupCreate::SelectByBoundings(Group& group, std::shared_ptr<GeometryImpl>& bounding)
{
    auto brush_model = bounding->GetBrushModel();
    if (!brush_model) {
        return;
    }

    auto& brushes = brush_model->GetBrushes();
    auto& props = m_props.GetProps();
    assert(props[TYPE].Val().type == VarType::Int);
    switch (static_cast<GroupType>(props[TYPE].Val().i))
    {
    case GroupType::Points:
    {
        auto& pts = m_geo_impl->GetAttr().GetPoints();
        for (size_t i = 0, n = pts.size(); i < n; ++i) {
            for (auto& b : brushes) {
                if (b.impl->GetGeometry()->IsContain(pts[i]->pos)) {
                    group.Add(i);
                    break;
                }
            }
        }
    }
        break;
    case GroupType::Vertices:
        break;
    case GroupType::Edges:
        break;
    case GroupType::Primitives:
        break;
    default:
        assert(0);
    }
}

void GroupCreate::SelectAll(Group& group)
{
    auto& props = m_props.GetProps();
    assert(props[TYPE].Val().type == VarType::Int);
    switch (static_cast<GroupType>(props[TYPE].Val().i))
    {
    case GroupType::Points:
    {
        auto& pts = m_geo_impl->GetAttr().GetPoints();
        std::vector<size_t> items(pts.size());
        for (size_t i = 0, n = pts.size(); i < n; ++i) {
            items[i] = i;
        }
        group.SetItems(items);
    }
        break;
    case GroupType::Vertices:
    {
        auto& vertices = m_geo_impl->GetAttr().GetVertices();
        std::vector<size_t> items(vertices.size());
        for (size_t i = 0, n = vertices.size(); i < n; ++i) {
            items[i] = i;
        }
        group.SetItems(items);
    }
        break;
    case GroupType::Edges:
        break;
    case GroupType::Primitives:
    {
        auto& prims = m_geo_impl->GetAttr().GetPrimtives();
        std::vector<size_t> items(prims.size());
        for (size_t i = 0, n = prims.size(); i < n; ++i) {
            items[i] = i;
        }
        group.SetItems(items);
    }
        break;
    default:
        assert(0);
    }
}

GroupMerge GroupCreate::GetMergeOP() const
{
    auto& props = m_props.GetProps();
    assert(props[MERGE].Val().type == VarType::String);
    auto str = static_cast<const char*>(props[MERGE].Val().p);
    if (strcmp(str, "replace")) {
        return GroupMerge::Replace;
    } else if (strcmp(str, "union")) {
        return GroupMerge::Union;
    } else {
        assert(0);
        return GroupMerge::Replace;
    }
}

}
}