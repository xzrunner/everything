#include "everything/node/Knife.h"
#include "everything/NodeHelper.h"

#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>
#include <node0/SceneNode.h>

namespace evt
{
namespace node
{

void Knife::Execute(TreeContext& ctx)
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

    if (Clip(*brush.impl)) {
        NodeHelper::BuildPolymesh(*m_scene_node, *brush_model);
        NodeHelper::StoreBrush(*m_scene_node, model_ext);
    }
}

void Knife::SetOrigin(const sm::vec3& origin)
{
    if (m_origin == origin) {
        return;
    }

    m_origin = origin;

    SetDirty(true);
}

void Knife::SetDirection(const sm::vec3& dir)
{
    if (m_direction == dir) {
        return;
    }

    m_direction = dir;

    SetDirty(true);
}

void Knife::SetKeepType(KeepType keep)
{
    if (m_keep == keep) {
        return;
    }

    m_keep = keep;

    SetDirty(true);
}

bool Knife::Clip(pm3::Polytope& poly) const
{
    he::Polyhedron::KeepType keep;
    switch (m_keep)
    {
    case KeepType::KeepAbove:
        keep = he::Polyhedron::KeepType::KeepAbove;
        break;
    case KeepType::KeepBelow:
        keep = he::Polyhedron::KeepType::KeepBelow;
        break;
    case KeepType::KeepAll:
        keep = he::Polyhedron::KeepType::KeepAll;
        break;
    default:
        assert(0);
    }

    sm::Plane plane(m_direction, m_origin);
    if (poly.GetHalfedge()->Clip(plane, keep)) {
        poly.BuildFromHalfedge();
        return true;
    } else {
        return false;
    }
}

}
}