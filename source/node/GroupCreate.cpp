#include "everything/node/GroupCreate.h"
#include "everything/NodeHelper.h"
#include "everything/TreeContext.h"

#include <SM_Calc.h>
#include <model/BrushModel.h>
#include <node0/SceneNode.h>

namespace evt
{
namespace node
{

void GroupCreate::Execute(TreeContext& ctx)
{
    auto obj = NodeHelper::GetInputSceneNode(*this, 0);
    if (!obj) {
        return;
    }
    m_scene_node = obj->Clone();
    auto polytope = NodeHelper::GetPolytope(*m_scene_node);
    assert(polytope);

    auto group = std::make_shared<pm3::Group>();
    group->name = m_name;
    polytope->AddGroup(group);

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

void GroupCreate::SetType(pm3::GroupType type)
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

void GroupCreate::SelectByNormals(pm3::Group& group)
{
    auto polytope = NodeHelper::GetPolytope(*m_scene_node);
    assert(polytope);
    switch (m_type)
    {
    case pm3::GroupType::Face:
    {
        auto& faces = polytope->Faces();
        for (size_t i = 0, n = faces.size(); i < n; ++i)
        {
            auto& face = faces[i];
            auto angle = sm::get_angle(sm::vec3(0, 0, 0), m_direction, face->plane.normal);
            if (angle <= m_spread_angle * SM_DEG_TO_RAD) {
                group.items.push_back(i);
            }
        }
    }
        break;
    case pm3::GroupType::Points:
        break;
    case pm3::GroupType::Edges:
        break;
    case pm3::GroupType::Vertices:
        break;
    }
}

}
}