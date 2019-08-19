#include "everything/node/Curve.h"
#include "everything/Geometry.h"

#include <geoshape/Polyline3D.h>

namespace evt
{
namespace node
{

void Curve::Execute(TreeContext& ctx)
{
    m_geo = std::make_shared<Geometry>(GeoAdaptor::DataType::Shape);
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

void Curve::BuildModel()
{
    if (!m_geo) {
        return;
    }
    if (m_vertices.size() < 2) {
        return;
    }

    std::shared_ptr<gs::Shape3D> shape = nullptr;
    if (m_vertices.front() == m_vertices.back()) {
        auto vertices(m_vertices);
        vertices.pop_back();
        shape = std::make_shared<gs::Polyline3D>(vertices, true);
    } else {
        shape = std::make_shared<gs::Polyline3D>(m_vertices, false);
    }
    m_geo->UpdateByShape(shape);
}

}
}