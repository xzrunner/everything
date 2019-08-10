#include "everything/node/GroupCreate.h"
#include "everything/NodeHelper.h"
#include "everything/BrushGroup.h"
#include "everything/TreeContext.h"

#include <SM_Calc.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>

namespace evt
{
namespace node
{

void GroupCreate::Execute(TreeContext& ctx)
{
    m_scene_node = NodeHelper::ClonePrevSceneObj(*this, SOURCE_OBJ_IDX);
    if (!m_scene_node) {
        return;
    }

    if (!m_group)
    {
        m_group = std::make_shared<BrushGroup>();

        m_group->name = m_name;

        auto brush_model = NodeHelper::GetBrushModel(*m_scene_node);
        m_group->parts.resize(brush_model->GetBrushes().size());
    }
    else
    {
        if (m_name != m_group->name) {
            m_name = m_group->name;
        }
    }

    // clear
    m_group->Clear();

    // insert selected to brush part
    if (m_keep_by_normals) {
        SelectByNormals();
    }
}

void GroupCreate::UpdateContext(TreeContext& ctx)
{
    if (m_group) {
        ctx.AddGroup(m_group);
    }
}

void GroupCreate::SetName(const std::string& name)
{
    if (m_name == name) {
        return;
    }

    m_name = name;
    if (m_group) {
        m_group->name = m_name;
    }
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

void GroupCreate::SelectByNormals()
{
    auto brush_model = NodeHelper::GetBrushModel(*m_scene_node);
    assert(brush_model);
    auto& brushes = brush_model->GetBrushes();

    switch (m_type)
    {
    case GroupType::Primitives:
        for (int i = 0, n = brushes.size(); i < n; ++i)
        {
            auto& brush = brushes[i];
            auto& part = m_group->parts[i];
            auto& faces = brush.impl->Faces();
            for (size_t j = 0, m = faces.size(); j < m; ++j)
            {
                auto& face = faces[j];
                auto angle = sm::get_angle(sm::vec3(0, 0, 0), m_direction, face->plane.normal);
                if (angle <= m_spread_angle * SM_DEG_TO_RAD) {
                    part.faces.push_back(j);
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