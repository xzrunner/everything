#include "everything/node/GroupCreate.h"
#include "everything/Geometry.h"
#include "everything/GeoAttrHelper.h"

#include <SM_Calc.h>

namespace evt
{
namespace node
{

void GroupCreate::Execute(TreeContext& ctx)
{
    m_geo.reset();

    auto prev_geo = GetInputGeo(IDX_SOURCE_OBJ);
    if (!prev_geo) {
        return;
    }

    m_geo = std::make_shared<Geometry>(*prev_geo);

    auto group = std::make_shared<Group>();
    group->name = m_name;
    m_geo->AddGroup(group);

    // insert selected to brush part
    if (m_keep_by_normals) {
        SelectByNormals(*group);
    }
}

void GroupCreate::UpdateContext(TreeContext& ctx)
{
    //if (m_group) {
    //    ctx.AddGroup(m_group);
    //}
}

void GroupCreate::SetName(const std::string& name)
{
    if (m_name == name) {
        return;
    }

    m_name = name;
}

void GroupCreate::SetType(GroupType type)
{
    if (m_type == type) {
        return;
    }

    m_type = type;

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
    switch (m_type)
    {
    case GroupType::Primitives:
    {
        auto& prims = m_geo->GetAttr().GetPrimtives();
        for (size_t i = 0, n = prims.size(); i < n; ++i)
        {
            auto& prim = prims[i];
            auto normal = GeoAttrHelper::CalcFaceNormal(*prim);
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
    }
}

}
}