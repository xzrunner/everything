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
    m_geo_impl = std::make_shared<GeometryImpl>(GeoPolyline(vertices));
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