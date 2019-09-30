#include "sop/node/PolyFill.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/GeoAttrRebuild.h"

#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>

namespace sop
{
namespace node
{

void PolyFill::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    auto brush_model = m_geo_impl->GetBrushModel();
    if (!brush_model) {
        return;
    }

    GeoAttrRebuild attr_rebuild(*m_geo_impl);

    for (auto& brush : brush_model->GetBrushes())
    {
        auto poly = brush.impl;
        poly->GetGeometry()->Fill();
        poly->BuildFromGeo();
    }
    m_geo_impl->UpdateByBrush(*brush_model);
}

}
}