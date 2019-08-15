#include "everything/node/Boolean.h"
#include "everything/Geometry.h"

#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>

namespace evt
{
namespace node
{

void Boolean::Execute(TreeContext& ctx)
{
    if (m_imports[IDX_A].conns.empty() ||
        m_imports[IDX_B].conns.empty()) {
        return;
    }
    auto node_a = m_imports[IDX_A].conns[0].node.lock();
    auto node_b = m_imports[IDX_B].conns[0].node.lock();
    if (!node_a || !node_b) {
        return;
    }
    auto geo_a = node_a->GetGeometry();
    auto geo_b = node_b->GetGeometry();
    if (!node_a || !node_b) {
        return;
    }
    auto brush_model_a = geo_a->GetBrushModel();
    auto brush_model_b = geo_b->GetBrushModel();
    if (!brush_model_a || !brush_model_b) {
        return;
    }
    auto& brushes_a = brush_model_a->GetBrushes();
    auto& brushes_b = brush_model_b->GetBrushes();
    assert(brushes_a.size() == 1 && brushes_b.size() == 1);
    auto brush_a = brushes_a[0].impl;
    auto brush_b = brushes_b[0].impl;
    if (!brush_a || !brush_b) {
        return;
    }
    auto he_a = brush_a->GetHalfedge();
    auto he_b = brush_b->GetHalfedge();
    if (!he_a || !he_b) {
        return;
    }

    m_geo = std::make_shared<Geometry>(Geometry::DataType::Brush);

    std::vector<model::BrushModel::Brush> brushes;
    switch (m_operator)
    {
    case Operator::Union:
        break;
    case Operator::Intersect:
    {
        auto poly = he_a->Intersect(*he_b);
        if (poly->GetFaces().Size() > 0) 
        {
            model::BrushModel::Brush brush;
            brush.impl = std::make_shared<pm3::Polytope>(poly);
            brushes.push_back(brush);
        }
    }
        break;
    case Operator::Subtract:
        break;
    default:
        assert(0);
    }

    if (!brushes.empty()) 
    {
        auto brush_model = std::make_unique<model::BrushModel>();
        brush_model->SetBrushes(brushes);
        m_geo->UpdateModel(*brush_model);
        m_geo->StoreBrush(brush_model);
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