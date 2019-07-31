#include "everything/node/PolyExtrude.h"
#include "everything/NodeHelper.h"

#include <polymesh3/Brush.h>
#include <model/BrushBuilder.h>
#include <node0/SceneNode.h>
#include <node3/CompModel.h>

namespace evt
{
namespace node
{

void PolyExtrude::ExecuteSelf()
{
    m_scene_node = NodeHelper::ClonePrevSceneObj(*this, 0);
    if (!m_scene_node) {
        return;
    }

    assert(m_scene_node && m_scene_node->HasSharedComp<n3::CompModel>());
    if (m_group)
    {
    }
    else
    {
        auto brush_model = NodeHelper::GetBrushModel(m_scene_node);
        assert(brush_model);
        auto& brushes = brush_model->GetBrushes();
        assert(brushes.size() == 1);
        auto& brush = brushes[0];
        for (auto& f : brush.impl->faces) {
            auto offset = f->plane.normal * m_distance / model::BrushBuilder::VERTEX_SCALE;
            for (auto& v : f->vertices) {
                brush.impl->vertices[v] += offset;
            }
        }

        NodeHelper::UpdateModelFromBrush(*m_scene_node, *brush_model);

        assert(m_scene_node && m_scene_node->HasSharedComp<n3::CompModel>());
        auto& src_cmodel = m_scene_node->GetSharedComp<n3::CompModel>();
        auto model = src_cmodel.GetModel();

        model::BrushBuilder::UpdateVBO(*model, *brush.impl, brush.desc);
    }
}

void PolyExtrude::SetGroup(const std::shared_ptr<model::BrushModel::BrushGroup>& group)
{
    m_group = group;

    Execute(false);
}

void PolyExtrude::SetDistance(float dist)
{
    m_distance = dist;

    Execute(false);
}

}
}