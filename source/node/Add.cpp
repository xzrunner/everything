#include "everything/node/Add.h"
#include "everything/GeometryImpl.h"
#include "everything/NodeHelper.h"

namespace evt
{
namespace node
{

void Add::Execute(Evaluator& eval, TreeContext& ctx)
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
    m_geo_impl->FromGeoShapes({ std::make_shared<GeoPolyline>(vertices) });

    // update point attrs
    if (prev_geo)
    {
        auto& prev_attr = prev_geo->GetAttr();
        auto& attr = m_geo_impl->GetAttr();
        attr.SetAttrDesc(GeoAttrType::Point,
            prev_attr.GetAttrDesc(GeoAttrType::Point));

        auto& prev_pts = prev_attr.GetPoints();
        auto& pts = attr.GetPoints();
        assert(prev_pts.size() <= pts.size());
        for (int i = 0, n = prev_pts.size(); i < n; ++i) {
            pts[i]->vars = prev_pts[i]->vars;
        }
        if (pts.size() > prev_pts.size())
        {
            auto default_vars = prev_attr.GetDefaultValues(GeoAttrType::Point);
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