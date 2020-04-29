#include "sop/node/Curve.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

#include <halfedge/Polyline.h>

namespace sop
{
namespace node
{

void Curve::Execute(const ur::Device& dev, Evaluator& eval)
{
    m_geo_impl = std::make_shared<GeometryImpl>(dev, GeoAdaptor::Type::Shape);
    BuildModel();
}

void Curve::BuildModel()
{
    //if (!m_geo_impl || m_vertices.size() < 2) {
    //    return;
    //}

    //std::vector<std::pair<he::TopoID, sm::vec3>> line_vertices;
    //std::vector<std::pair<he::TopoID, std::vector<size_t>>> line_polylines;
    //line_vertices.reserve(m_vertices.size());
    //for (auto& v : m_vertices) {
    //    line_vertices.push_back({ he::TopoID(), v });
    //}
    //std::vector<size_t> indices(m_vertices.size());
    //for (size_t i = 0, n = m_vertices.size(); i < n; ++i) {
    //    indices[i] = i;
    //}
    //// check curve closed
    //if (m_vertices.front() == m_vertices.back()) {
    //    line_vertices.pop_back();
    //    indices.back() = 0;
    //}
    //line_polylines.push_back({ he::TopoID(), { indices } });
    //auto line = std::make_shared<he::Polyline>(line_vertices, line_polylines);
    //m_geo_impl->SetTopoLines({ line });
}

}
}