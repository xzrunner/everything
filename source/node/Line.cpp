#include "sop/node/Line.h"
#include "sop/GeometryImpl.h"
#include "sop/GeoAdaptor.h"
#include "sop/NodeHelper.h"

#include <halfedge/Polyline.h>

namespace sop
{
namespace node
{

void Line::Execute(Evaluator& eval)
{
    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Shape);
    BuildModel();
}

void Line::BuildModel()
{
    if (!m_geo_impl) {
        return;
    }
    if (m_points < 2) {
        return;
    }

    std::vector<std::pair<he::TopoID, sm::vec3>> line_vertices;
    std::vector<std::pair<he::TopoID, std::vector<size_t>>> line_polylines;
    line_vertices.reserve(m_points);
    std::vector<size_t> indices(m_points);

    auto dt = m_direction * m_len / static_cast<float>(m_points - 1);
    auto v = m_origin;
    for (int i = 0; i < m_points; ++i)
    {
        line_vertices.push_back({ he::TopoID(), v });
        v += dt;
        indices[i] = i;
    }
    line_polylines.push_back({ he::TopoID(), { indices } });

    auto line = std::make_shared<he::Polyline>(line_vertices, line_polylines);
    m_geo_impl->SetTopoLines({ line });
}

}
}