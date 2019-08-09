#include "everything/node/PolyExtrude.h"
#include "everything/NodeHelper.h"
#include "everything/BrushGroup.h"
#include "everything/TreeContext.h"

#include <polymesh3/Brush.h>
#include <model/BrushBuilder.h>
#include <model/BrushModel.h>
#include <node0/SceneNode.h>

namespace evt
{
namespace node
{

void PolyExtrude::Execute(TreeContext& ctx)
{
    m_scene_node = NodeHelper::ClonePrevSceneObj(*this, 0);
    if (!m_scene_node) {
        return;
    }

    auto group = ctx.QueryGroup(m_group_name);

    assert(m_scene_node);
    auto old_brush_model = NodeHelper::GetBrushModel(*m_scene_node);
    assert(old_brush_model);
    auto model_ext = old_brush_model->Clone();
    auto brush_model = static_cast<model::BrushModel*>(model_ext.get());
    auto& brushes = brush_model->GetBrushes();

    if (group)
    {
        bool dirty = false;

        assert(brushes.size() == group->parts.size());
        for (int i = 0, n = group->parts.size(); i < n; ++i)
        {
            auto& part = group->parts[i];
            if (part.faces.empty()) {
                continue;
            }

            dirty = true;
            for (auto& f : part.faces) {
                ExtrudeFace(*brushes[i].impl, f, m_distance);
            }
        }

        if (dirty) {
            NodeHelper::BuildPolymesh(*m_scene_node, *brush_model);
            NodeHelper::StoreBrush(*m_scene_node, model_ext);
        }
    }
    else
    {
        for (auto& brush : brushes) {
            for (size_t i = 0; i < brush.impl->faces.size(); ++i) {
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

void PolyExtrude::ExtrudeFace(pm3::Brush& brush, size_t face_idx, float dist)
{
    auto face = brush.faces[face_idx];
    auto offset = face->plane.normal * dist;
    for (auto& v : face->vertices) {
        brush.vertices[v] += offset;
    }
}

}
}