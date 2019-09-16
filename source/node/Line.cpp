#include "everything/node/Line.h"
#include "everything/GeometryImpl.h"
#include "everything/GeoAdaptor.h"

namespace evt
{
namespace node
{

void Line::Execute(Evaluator& eval, TreeContext& ctx)
{
    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Shape);
    BuildModel();
}

void Line::SetOrigin(const sm::vec3& ori)
{
    if (m_origin == ori) {
        return;
    }

    m_origin = ori;
    BuildModel();

    SetDirty(true);
}

void Line::SetDirection(const sm::vec3& dir)
{
    if (m_direction == dir) {
        return;
    }

    m_direction = dir;
    BuildModel();

    SetDirty(true);
}

void Line::SetLength(float len)
{
    if (m_length == len) {
        return;
    }

    m_length = len;
    BuildModel();

    SetDirty(true);
}

void Line::SetPoints(size_t num)
{
    if (m_points == num) {
        return;
    }

    m_points = num;
    BuildModel();

    SetDirty(true);
}

void Line::BuildModel()
{
    if (!m_geo_impl) {
        return;
    }
    if (m_points < 2) {
        return;
    }

    std::vector<sm::vec3> vertices;
    vertices.reserve(m_points);
    auto dt = m_direction * m_length / static_cast<float>(m_points - 1);
    auto v = m_origin;
    for (size_t i = 0; i < m_points; ++i) {
        vertices.push_back(v);
        v += dt;
    }
    m_geo_impl->FromGeoShapes({ std::make_unique<GeoPolyline>(vertices) });
}}
}
