#include "everything/node/PolyExtrude.h"
#include "everything/NodeHelper.h"
#include "everything/TreeContext.h"

#include <polymesh3/Geometry.h>
#include <model/BrushBuilder.h>
#include <model/BrushModel.h>
#include <node0/SceneNode.h>

namespace evt
{
namespace node
{

void PolyExtrude::Execute(TreeContext& ctx)
{
    auto obj = NodeHelper::GetInputSceneNode(*this, 0);
    if (!obj) {
        return;
    }
    m_scene_node = obj->Clone();
    auto old_brush_model = NodeHelper::GetBrushModel(*m_scene_node);
    assert(old_brush_model);
    auto model_ext = old_brush_model->Clone();
    auto brush_model = static_cast<model::BrushModel*>(model_ext.get());
    auto& brushes = brush_model->GetBrushes();
    assert(brushes.size() == 1);
    auto& brush = brushes[0];

    auto group = brush.impl->QueryGroup(m_group_name);
    if (group)
    {
        assert(group->type == pm3::GroupType::Face);

        if (!group->items.empty())
        {
            for (auto& i : group->items) {
                ExtrudeFace(*brush.impl, i, m_distance);
            }

            NodeHelper::BuildPolymesh(*m_scene_node, *brush_model);
            NodeHelper::StoreBrush(*m_scene_node, model_ext);
        }
    }
    else
    {
        for (auto& brush : brushes) {
            for (size_t i = 0; i < brush.impl->Faces().size(); ++i) {
                ExtrudeFace(*brush.impl, i, m_distance);
            }
        }

        NodeHelper::BuildPolymesh(*m_scene_node, *brush_model);
        NodeHelper::StoreBrush(*m_scene_node, model_ext);
    }
}

void PolyExtrude::SetDistance(float dist)
{
    if (m_distance == dist) {
        return;
    }

    m_distance = dist;

    SetDirty(true);
}

void PolyExtrude::ExtrudeFace(pm3::Polytope& poly, size_t face_idx, float dist)
{
    auto face = poly.Faces()[face_idx];
    auto offset = face->plane.normal * dist;
    for (auto& v : face->points) {
        poly.Points()[v] += offset;
    }
}

}
}