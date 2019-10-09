#include "sop/node/Add.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

#include <halfedge/Polyline.h>

namespace sop
{
namespace node
{

void Add::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    std::vector<sm::vec3> vertices;

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (prev_geo)
    {
        auto& prev_points = prev_geo->GetAttr().GetPoints();
        vertices.reserve(prev_points.size());
        for (auto& p : prev_points) {
            vertices.push_back(p->pos);
        }
    }

    if (vertices.empty() && m_points.empty()) {
        return;
    }
    std::copy(m_points.begin(), m_points.end(), std::back_inserter(vertices));

    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Shape);
    std::vector<std::pair<he::TopoID, sm::vec3>> line_vertices;
    std::vector<std::pair<he::TopoID, std::vector<size_t>>> line_polylines;
    line_vertices.reserve(vertices.size());
    for (auto& v : vertices) {
        line_vertices.push_back({ he::TopoID(), v });
    }
    std::vector<size_t> indices(vertices.size());
    for (size_t i = 0, n = vertices.size(); i < n; ++i) {
        indices[i] = i;
    }
    line_polylines.push_back({ he::TopoID(), { indices } });
    auto line = std::make_shared<he::Polyline>(line_vertices, line_polylines);
    m_geo_impl->SetTopoLines({ line });

    // update point attrs
    if (prev_geo)
    {
        auto& prev_attr = prev_geo->GetAttr();
        auto& attr = m_geo_impl->GetAttr();
        attr.SetAttrDesc(GeoAttrClass::Point,
            prev_attr.GetAttrDesc(GeoAttrClass::Point));

        auto& prev_pts = prev_attr.GetPoints();
        auto& pts = attr.GetPoints();
        assert(prev_pts.size() <= pts.size());
        for (int i = 0, n = prev_pts.size(); i < n; ++i) {
            pts[i]->vars = prev_pts[i]->vars;
        }
        if (pts.size() > prev_pts.size())
        {
            auto default_vars = prev_attr.GetDefaultValues(GeoAttrClass::Point);
            for (int i = prev_pts.size(), n = pts.size(); i < n; ++i) {
                pts[i]->vars = default_vars;
            }
        }
    }
}

void Add::SetPoints(const std::vector<sm::vec3>& points)
{
    if (m_points == points) {
        return;
    }

    m_points = points;

    SetDirty(true);
}

}
}