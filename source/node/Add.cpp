#include "sop/node/Add.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/GeoAttrRebuild.h"

#include <halfedge/Polyline.h>

namespace sop
{
namespace node
{

void Add::Execute(const ur2::Device& dev, Evaluator& eval)
{
    m_geo_impl.reset();

    auto vertices = GetPrevVertices();
    if (vertices.empty() && m_points.empty()) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(dev, GeoAdaptor::Type::Shape);

    assert(m_points_enable.size() == m_points.size());
    for (size_t i = 0, n = m_points.size(); i < n; ++i) {
        if (m_points_enable[i]) {
            vertices.push_back({ he::TopoID(), m_points[i] });
        }
    }

    std::vector<std::pair<he::TopoID, sm::vec3>> line_vertices;
    std::vector<std::pair<he::TopoID, std::vector<size_t>>> line_polylines;
    line_vertices.reserve(vertices.size());
    for (auto& v : vertices) {
        line_vertices.push_back(v);
    }
    std::vector<size_t> indices(vertices.size());
    for (size_t i = 0, n = vertices.size(); i < n; ++i) {
        indices[i] = i;
    }
    line_polylines.push_back({ he::TopoID(), { indices } });
    auto line = std::make_shared<he::Polyline>(line_vertices, line_polylines);
    m_geo_impl->SetTopoLines({ line });

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (prev_geo) {
        GeoAttrRebuild::Copy(*m_geo_impl, *prev_geo);
    }
}

void Add::SetStdSwitcher1(int sel)
{
    assert(sel == 1);
    m_stdswitcher1 = sel;
}

void Add::SetSwitcher1(int sel)
{
    assert(sel == 1);
    m_switcher1 = 1;
}

void Add::SetPointsNum(size_t num)
{
    m_points_enable.resize(num, false);
    m_points.resize(num, sm::vec3(0, 0, 0));
}

size_t Add::GetPointsNum() const
{
    assert(m_points_enable.size() == m_points.size());
    return m_points.size();
}

std::vector<std::pair<he::TopoID, sm::vec3>>
Add::GetPrevVertices() const
{
    std::vector<std::pair<he::TopoID, sm::vec3>> vertices;

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return vertices;
    }

    auto& prev_points = prev_geo->GetAttr().GetPoints();
    vertices.reserve(prev_points.size());
    for (auto& p : prev_points) {
        vertices.push_back({ p->topo_id, p->pos });
    }

    return vertices;
}

}
}