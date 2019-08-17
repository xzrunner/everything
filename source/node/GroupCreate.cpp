#include "everything/node/GroupCreate.h"
#include "everything/TreeContext.h"

#include <SM_Calc.h>
#include <model/BrushModel.h>
#include <node0/SceneNode.h>
#include <polymesh3/Geometry.h>

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

    auto group = std::make_shared<Geometry::Group>();
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

void GroupCreate::SetType(Geometry::GroupType type)
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

void GroupCreate::SelectByNormals(Geometry::Group& group)
{
    switch (m_type)
    {
    case Geometry::GroupType::Face:
    {
        m_geo->TraverseFaces([&](pm3::Polytope& poly, size_t face_idx, bool& dirty)->bool
        {
            auto& faces = poly.Faces();
            auto& face = faces[face_idx];
            auto angle = sm::get_angle(sm::vec3(0, 0, 0), m_direction, face->plane.normal);
            if (angle <= m_spread_angle * SM_DEG_TO_RAD) {
                group.items.push_back(face_idx);
            }
            dirty = false;
            return true;
        });
    }
        break;
    case Geometry::GroupType::Points:
        break;
    case Geometry::GroupType::Edges:
        break;
    case Geometry::GroupType::Vertices:
        break;
    }
}

}
}