#include "everything/node/Curve.h"
#include "everything/GeometryImpl.h"

namespace evt
{
namespace node
{

void Curve::Execute(TreeContext& ctx)
{
    m_geo_impl = std::make_shared<GeometryImpl>(GeoShapeType::Polyline);
    BuildModel();
}

void Curve::SetVertices(const std::vector<sm::vec3>& vertices)
{
    if (m_vertices == vertices) {
        return;
    }

    m_vertices = vertices;
    BuildModel();

    SetDirty(true);
}

// todo: test close
void Curve::BuildModel()
{
    if (m_geo_impl && m_vertices.size() >= 2) {
        m_geo_impl->FromGeoShape(GeoPolyline(m_vertices));
    }
}

}
}