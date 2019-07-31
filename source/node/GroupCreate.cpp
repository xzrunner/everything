#include "everything/node/GroupCreate.h"
#include "everything/NodeHelper.h"

#include <SM_Calc.h>
#include <polymesh3/Brush.h>

namespace evt
{
namespace node
{

void GroupCreate::SetName(const std::string& name)
{
    if (m_name == name) {
        return;
    }

    m_name = name;
    if (m_group) {
        m_group->name = name;
    }
}

void GroupCreate::SetType(GroupType type)
{
    if (m_type == type) {
        return;
    }

    m_type = type;

    Execute(false);
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

    Execute(false);
}

void GroupCreate::DisableKeepByNormals()
{
    if (!m_keep_by_normals) {
        return;
    }

    m_keep_by_normals = false;

    Execute(false);
}

void GroupCreate::ExecuteSelf()
{
    m_scene_node = NodeHelper::ClonePrevSceneObj(*this, SOURCE_OBJ_IDX);
    if (!m_scene_node) {
        return;
    }

    if (!m_group) {
        m_group = std::make_shared<model::BrushModel::BrushGroup>();
    }

    m_group->name = m_name;

    // parent
    auto brush_model = NodeHelper::GetBrushModel(m_scene_node);
    assert(brush_model);
    auto& brushes = brush_model->GetBrushes();
    assert(brushes.size() == 1);
    m_group->part.parent = brushes[0].impl;

    // clear
    m_group->part.vertices.clear();
    m_group->part.edges.clear();
    m_group->part.faces.clear();

    // insert selected to brush part
    if (m_keep_by_normals) {
        SelectByNormals();
    }

    //// update scene node
    //assert(m_scene_node->HasSharedComp<n3::CompModel>());
    //auto& dst_cmodel = m_scene_node->GetSharedComp<n3::CompModel>();
    //assert(dst_cmodel.GetModel()->ext && dst_cmodel.GetModel()->ext->Type() == model::EXT_BRUSH);
    //brush_model->SetBrushes(brushes);
    //brush_model->SetBrushGroups({ m_group });
}

void GroupCreate::SelectByNormals()
{
    assert(m_group->part.parent);
    switch (m_type)
    {
    case GroupType::Primitives:
        for (auto& f : m_group->part.parent->faces) {
            if (sm::get_angle(sm::vec3(0, 0, 0), m_direction, f->plane.normal) <= m_spread_angle * SM_DEG_TO_RAD) {
                m_group->part.faces.push_back(f);
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