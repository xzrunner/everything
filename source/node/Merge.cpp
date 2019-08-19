#include "everything/node/Merge.h"
#include "everything/Geometry.h"

#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>

namespace evt
{
namespace node
{

void Merge::Execute(TreeContext& ctx)
{
    m_geo = std::make_shared<Geometry>(GeoAdaptor::DataType::Brush);

    std::vector<std::shared_ptr<Geometry>> children;
    for (auto& port : m_imports)
    {
        if (port.conns.empty()) {
            continue;
        }

        assert(port.conns.size() == 1);
        auto src_obj = port.conns[0].node.lock();
        if (!src_obj) {
            continue;
        }

        children.push_back(src_obj->GetGeometry());
    }

    // build brush
    model::BrushModel::Brush brush;
    brush.impl = std::make_shared<pm3::Polytope>();
    for (auto& c : children)
    {
        assert(c);
        m_geo->Combine(*c, brush.impl->Faces().size());
        for (auto& b : c->GetBrushModel()->GetBrushes()) {
            brush.impl->Combine(*b.impl);
        }
    }

    auto brush_model = std::make_unique<model::BrushModel>();
    std::vector<model::BrushModel::Brush> brushes;
    brushes.push_back(brush);
    brush_model->SetBrushes(brushes);

    m_geo->UpdateByBrush(*brush_model);
    m_geo->StoreBrush(brush_model);
}

}
}