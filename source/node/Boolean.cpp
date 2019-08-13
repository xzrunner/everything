#include "everything/node/Boolean.h"
#include "everything/NodeHelper.h"

#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>
#include <node0/SceneNode.h>

namespace evt
{
namespace node
{

void Boolean::Execute(TreeContext& ctx)
{
    auto node_a = NodeHelper::GetInputSceneNode(*this, IDX_A);
    auto node_b = NodeHelper::GetInputSceneNode(*this, IDX_B);
    if (!node_a || !node_b) {
        return;
    }

    auto poly_a = NodeHelper::GetPolytope(*node_a);
    auto poly_b = NodeHelper::GetPolytope(*node_b);
    if (!poly_a || !poly_b) {
        return;
    }

    auto he_a = poly_a->GetHalfedge();
    auto he_b = poly_b->GetHalfedge();
    if (!he_a || !he_b) {
        return;
    }

    m_scene_node = node_a->Clone();
    auto old_brush_model = NodeHelper::GetBrushModel(*m_scene_node);
    assert(old_brush_model);
    auto model_ext = old_brush_model->Clone();
    auto brush_model = static_cast<model::BrushModel*>(model_ext.get());
    auto& brushes = brush_model->GetBrushes();
    assert(brushes.size() == 1);
    auto& brush = brushes[0];

    bool dirty = false;
    switch (m_operator)
    {
    case Operator::Union:
        break;
    case Operator::Intersect:
    {
        auto poly = he_a->Intersect(*he_b);
        if (poly->GetFaces().Size() > 0) {
            const_cast<model::BrushModel::Brush&>(brush).impl
                = std::make_shared<pm3::Polytope>(poly);
            dirty = true;
        }
    }
        break;
    case Operator::Subtract:
        break;
    default:
        assert(0);
    }

    if (dirty) {
        NodeHelper::BuildPolymesh(*m_scene_node, *brush_model);
        NodeHelper::StoreBrush(*m_scene_node, model_ext);
    }
}

void Boolean::SetOperator(Operator op)
{
    if (m_operator == op) {
        return;
    }

    m_operator = op;

    SetDirty(true);
}

}
}