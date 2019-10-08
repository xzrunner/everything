#include "sop/node/Fuse.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/GeoAttrRebuild.h"
#include "sop/GroupRebuild.h"

#include <halfedge/Polyhedron.h>
#include <halfedge/Polyline.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>
#include <SM_Calc.h>

namespace sop
{
namespace node
{

void Fuse::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    switch (m_geo_impl->GetAdaptorType())
    {
    case GeoAdaptor::Type::Brush:
        FuseBrush();
        break;
    case GeoAdaptor::Type::Shape:
        FuseShape();
        break;
    default:
        assert(0);
    }
}

void Fuse::SetDistance(float dist)
{
    if (m_distance == dist) {
        return;
    }

    m_distance = dist;

    SetDirty(true);
}

void Fuse::FuseBrush()
{
    auto brush_model = m_geo_impl->GetBrushModel();
    if (!brush_model) {
        return;
    }

    GroupRebuild group_rebuild(*m_geo_impl);
    GeoAttrRebuild attr_rebuild(*m_geo_impl);

    auto& brushes = brush_model->GetBrushes();
    std::vector<he::PolyhedronPtr> src;
    src.reserve(brushes.size());
    for (auto& b : brushes) {
        src.push_back(b.impl->GetGeometry());
    }
    auto dst = he::Polyhedron::Fuse(src, m_distance);
    model::BrushModel::Brush brush;
    brush.impl = std::make_shared<pm3::Polytope>(dst);
    brush_model->SetBrushes({ brush });

    m_geo_impl->UpdateByBrush(*brush_model);
}

void Fuse::FuseShape()
{
    GroupRebuild group_rebuild(*m_geo_impl);
    GeoAttrRebuild attr_rebuild(*m_geo_impl);

    auto lines = m_geo_impl->GetTopoLines();
    for (auto line : lines) {
        line->Fuse(m_distance);
    }
    m_geo_impl->SetTopoLines(lines);
}

}
}