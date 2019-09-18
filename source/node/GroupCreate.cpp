#include "everything/node/GroupCreate.h"
#include "everything/GeometryImpl.h"
#include "everything/NodeHelper.h"
#include "everything/EvalContext.h"
#include "everything/Evaluator.h"

#include <SM_Calc.h>

namespace evt
{
namespace node
{

void GroupCreate::Execute(Evaluator& eval, TreeContext& ctx)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, IDX_SOURCE_OBJ);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    auto& group_mgr = m_geo_impl->GetGroup();

    auto group = std::make_shared<Group>();
    group->name = m_group_name;
    group->type = m_group_type;

    if (m_base_group) {
        group->items = NodeHelper::SelectGeoByExpr(m_group_type, eval, *this, m_base_group_expr);
    } else if (m_keep_by_normals) {
        SelectByNormals(*group);
    } else {
        SelectAll(*group);
    }

    group_mgr.Add(group, m_merge_op);
    m_group_name = group->name;
}

void GroupCreate::UpdateContext(TreeContext& ctx)
{
    //if (m_group) {
    //    ctx.AddGroup(m_group);
    //}
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
    if (m_group_type == type) {
        return;
    }

    m_group_type = type;

    SetDirty(true);
}

void GroupCreate::SetGroupMerge(GroupMerge merge)
{
    if (m_merge_op == merge) {
        return;
    }

    m_merge_op = merge;

    SetDirty(true);
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
    if (!m_base_group) {
        return;
    }

    m_base_group = false;

    SetDirty(true);
}

void GroupCreate::EnableKeepByNormals(const sm::vec3& direction, float spread_angle)
{
    if (m_keep_by_normals &&
        m_direction == direction &&
        m_spread_angle == spread_angle) {
        return;
    }

    m_keep_by_normals = true;
    m_direction       = direction;
    m_spread_angle    = spread_angle;

    SetDirty(true);
}

void GroupCreate::DisableKeepByNormals()
{
    if (!m_keep_by_normals) {
        return;
    }

    m_keep_by_normals = false;

    SetDirty(true);
}

void GroupCreate::SelectByNormals(Group& group)
{
    switch (m_group_type)
    {
    case GroupType::Primitives:
    {
        auto& prims = m_geo_impl->GetAttr().GetPrimtives();
        for (size_t i = 0, n = prims.size(); i < n; ++i)
        {
            auto normal = prims[i]->CalcNormal();
            auto angle = sm::get_angle(sm::vec3(0, 0, 0), m_direction, normal);
            if (angle <= m_spread_angle * SM_DEG_TO_RAD) {
                group.items.push_back(i);
            }
        }
    }
        break;
    case GroupType::Points:
        break;
    case GroupType::Edges:
        break;
    case GroupType::Vertices:
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
        group.items.resize(pts.size());
        for (size_t i = 0, n = pts.size(); i < n; ++i) {
            group.items[i] = i;
        }
    }
        break;
    case GroupType::Vertices:
    {
        auto& vertices = m_geo_impl->GetAttr().GetVertices();
        group.items.resize(vertices.size());
        for (size_t i = 0, n = vertices.size(); i < n; ++i) {
            group.items[i] = i;
        }
    }
        break;
    case GroupType::Edges:
        break;
    case GroupType::Primitives:
    {
        auto& prims = m_geo_impl->GetAttr().GetPrimtives();
        group.items.resize(prims.size());
        for (size_t i = 0, n = prims.size(); i < n; ++i) {
            group.items[i] = i;
        }
    }
        break;
    default:
        assert(0);
    }
}

}
}