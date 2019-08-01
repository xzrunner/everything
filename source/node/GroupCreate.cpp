#include "everything/node/GroupCreate.h"
#include "everything/NodeHelper.h"
#include "everything/BrushGroup.h"
#include "everything/TreeContext.h"

#include <SM_Calc.h>
#include <model/BrushModel.h>
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
        m_group->name = m_name;
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

void GroupCreate::UpdateCtxSelf(TreeContext& ctx)
{
    if (m_group) {
        ctx.AddGroup(m_group);
    }
}

void GroupCreate::ExecuteSelf()
{
    m_scene_node = NodeHelper::ClonePrevSceneObj(*this, SOURCE_OBJ_IDX);
    if (!m_scene_node) {
        return;
    }

    if (!m_group) {
        m_group = std::make_shared<BrushGroup>();
        m_group->name = m_name;
    } else {
        if (m_name != m_group->name) {
            m_name = m_group->name;
        }
    }

    // clear
    m_group->vertices.clear();
    m_group->edges.clear();
    m_group->faces.clear();

    // insert selected to brush part
    if (m_keep_by_normals) {
        SelectByNormals();
    }
}

void GroupCreate::SelectByNormals()
{
    auto brush = NodeHelper::GetBrush(m_scene_node);
    assert(brush);

    switch (m_type)
    {
    case GroupType::Primitives:
        for (size_t i = 0, n = brush->faces.size(); i < n; ++i)
        {
            auto& face = brush->faces[i];
            auto angle = sm::get_angle(sm::vec3(0, 0, 0), m_direction, face->plane.normal);
            if (angle <= m_spread_angle * SM_DEG_TO_RAD) {
                m_group->faces.push_back(i);
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