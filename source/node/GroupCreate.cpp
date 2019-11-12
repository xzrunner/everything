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
    group->SetName(m_group_name);
    group->SetType(m_group_type);

    if (m_base_group)
    {
        if (m_base_group_expr.empty()) {
            SelectAll(*group);
        } else {
            group->SetItems(NodeHelper::SelectGeoByExpr(m_group_type, eval, *this, m_base_group_expr));
        }
    }
    else if (m_keep_in_bounding)
    {
        auto b_geo = NodeHelper::GetInputGeo(*this, IDX_BOUNDING_OBJ);
        if (b_geo) {
            SelectByBoundings(*group, b_geo);
        }
    }
    else if (m_keep_by_normals)
    {
        SelectByNormals(*group);
    }

    group_mgr.Add(group, m_merge_op);
    m_group_name = group->GetName();
}

void GroupCreate::SetGroupName(const std::string& name)
{
    if (m_group_name == name) {
        return;
    }

    m_group_name = name;
}

void GroupCreate::SetGroupType(GroupType type)
{
    NODE_PROP_SET(m_group_type, type);
}

void GroupCreate::SetGroupMerge(GroupMerge merge)
{
    NODE_PROP_SET(m_merge_op, merge);
}

void GroupCreate::EnableBaseGroup(const std::string& expr)
{
    if (m_base_group &&
        m_base_group_expr == expr) {
        return;
    }

    m_base_group      = true;
    m_base_group_expr = expr;

    SetDirty(true);
}

void GroupCreate::DisableBaseGroup()
{
    NODE_PROP_SET(m_base_group, false);
}

void GroupCreate::EnableKeepInBounding()
{
    NODE_PROP_SET(m_keep_in_bounding, true);
}

void GroupCreate::DisableKeepInBounding()
{
    NODE_PROP_SET(m_keep_in_bounding, false);
}

void GroupCreate::EnableKeepByNormals(const sm::vec3& direction, float spread_angle)
{
    bool dir_dirty = false;

    if (m_props.SetValue(DIR_X, Variable(direction.x))) {
        dir_dirty = true;
    }
    if (m_props.SetValue(DIR_Y, Variable(direction.y))) {
        dir_dirty = true;
    }
    if (m_props.SetValue(DIR_Z, Variable(direction.z))) {
        dir_dirty = true;
    }

    if (m_keep_by_normals &&
        !dir_dirty &&
        m_spread_angle == spread_angle) {
        return;
    }

    m_keep_by_normals = true;
    m_spread_angle    = spread_angle;

    SetDirty(true);
}

void GroupCreate::DisableKeepByNormals()
{
    NODE_PROP_SET(m_keep_by_normals, false);
}

void GroupCreate::InitProps()
{
    m_props.Assign(DIR_X, PropNames[DIR_X], Variable(0.0f));
    m_props.Assign(DIR_Y, PropNames[DIR_Y], Variable(0.0f));
    m_props.Assign(DIR_Z, PropNames[DIR_Z], Variable(1.0f));
}

void GroupCreate::SelectByNormals(Group& group)
{
    switch (m_group_type)
    {
    case GroupType::Points:
        break;
    case GroupType::Vertices:
        break;
    case GroupType::Edges:
        break;
    case GroupType::Primitives:
    {
        auto& prims = m_geo_impl->GetAttr().GetPrimtives();
        for (size_t i = 0, n = prims.size(); i < n; ++i)
        {
            auto normal = prims[i]->CalcNormal();
            auto& props = m_props.GetProps();
            const sm::vec3 dir = sm::vec3(props[DIR_X].Val().f, props[DIR_Y].Val().f, props[DIR_Z].Val().f) * 0.5f;
            auto angle = sm::get_angle(sm::vec3(0, 0, 0), dir, normal);
            if (angle <= m_spread_angle * SM_DEG_TO_RAD) {
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
    switch (m_group_type)
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
    switch (m_group_type)
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

}
}