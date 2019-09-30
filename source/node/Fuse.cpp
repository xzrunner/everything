#include "everything/node/Fuse.h"
#include "everything/GeometryImpl.h"
#include "everything/NodeHelper.h"
#include "everything/GeoAttrRebuild.h"
#include "everything/GroupRebuild.h"

#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>
#include <SM_Calc.h>

namespace evt
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
    // todo: group and attr

    auto shapes = m_geo_impl->ToGeoShapes();
    for (auto& shape : shapes)
    {
        if (shape->Type() != GeoShapeType::Polyline) {
            continue;
        }

        auto& src = std::static_pointer_cast<GeoPolyline>(shape)->GetVertices();
        if (src.size() < 2) {
            continue;
        }

        std::vector<sm::vec3> dst;
        dst.push_back(src[0]);
        for (size_t i = 1, n = src.size(); i < n; ++i)
        {
            float dist = sm::dis_pos3_to_pos3(src[i - 1], src[i]);
            if (dist >= m_distance) {
                dst.push_back(src[i]);
            }
        }
        shape = std::make_shared<GeoPolyline>(dst);
    }
    m_geo_impl->FromGeoShapes(shapes);
}

}
}