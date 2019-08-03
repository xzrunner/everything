#include "everything/node/PolyExtrude.h"
#include "everything/NodeHelper.h"
#include "everything/BrushGroup.h"
#include "everything/TreeContext.h"

#include <polymesh3/Brush.h>
#include <model/BrushBuilder.h>
#include <model/BrushModel.h>
#include <node0/SceneNode.h>
#include <node3/CompModel.h>

namespace evt
{
namespace node
{

void PolyExtrude::BeforeUpdateContext()
{
    m_group = nullptr;
}

void PolyExtrude::SetDistance(float dist)
{
    if (m_distance == dist) {
        return;
    }

    m_distance = dist;

    Execute(false);
}

void PolyExtrude::ExecuteSelf()
{
    m_scene_node = NodeHelper::ClonePrevSceneObj(*this, 0);
    if (!m_scene_node) {
        return;
    }

    assert(m_scene_node && m_scene_node->HasSharedComp<n3::CompModel>());
    auto brush_model = NodeHelper::GetBrushModel(*m_scene_node);
    assert(brush_model);
    auto& brushes = brush_model->GetBrushes();

    if (m_group)
    {
        bool dirty = false;

        assert(brushes.size() == m_group->parts.size());
        for (int i = 0, n = m_group->parts.size(); i < n; ++i)
        {
            auto& part = m_group->parts[i];
            if (part.faces.empty()) {
                continue;
            }

            dirty = true;
            for (auto& f : part.faces) {
                ExtrudeFace(*brushes[i].impl, f, m_distance);
            }
        }

        if (dirty)
        {
            NodeHelper::UpdateModelFromBrush(*m_scene_node, *brush_model);

            assert(m_scene_node && m_scene_node->HasSharedComp<n3::CompModel>());
            auto& src_cmodel = m_scene_node->GetSharedComp<n3::CompModel>();
            auto model = src_cmodel.GetModel();

            model::BrushBuilder::UpdateVBO(*model, *brush_model);
        }
    }
    else
    {
        for (auto& brush : brushes) {
            for (size_t i = 0; i < brush.impl->faces.size(); ++i) {
                ExtrudeFace(*brush.impl, i, m_distance);
            }
        }

        NodeHelper::UpdateModelFromBrush(*m_scene_node, *brush_model);

        assert(m_scene_node && m_scene_node->HasSharedComp<n3::CompModel>());
        auto& src_cmodel = m_scene_node->GetSharedComp<n3::CompModel>();
        auto model = src_cmodel.GetModel();

        model::BrushBuilder::UpdateVBO(*model, *brush_model);
    }
}

void PolyExtrude::UpdateCtxSelf(TreeContext& ctx)
{
    auto group = ctx.QueryGroup(m_group_name);
    if (group) {
        m_group = group;
    }
}

void PolyExtrude::ExtrudeFace(pm3::Brush& brush, size_t face_idx, float dist)
{
    auto face = brush.faces[face_idx];
    auto offset = face->plane.normal * dist / model::BrushBuilder::VERTEX_SCALE;
    for (auto& v : face->vertices) {
        brush.vertices[v] += offset;
    }
}

}
}