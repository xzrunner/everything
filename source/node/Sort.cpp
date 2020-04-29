#include "sop/node/Sort.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

namespace sop
{
namespace node
{

void Sort::Execute(const ur::Device& dev, Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    auto& attr = m_geo_impl->GetAttr();
    auto& pts = attr.GetPoints();

    std::vector<size_t> order;

    switch (m_key)
    {
    case Key::X:
    case Key::Y:
    case Key::Z:
    {
        order.resize(pts.size());
        for (size_t i = 0, n = pts.size(); i < n; ++i) {
            order[i] = i;
        }
        std::sort(order.begin(), order.end(), PointCmp(pts, m_key));
    }
        break;
    case Key::Shift:
    {
        order.resize(pts.size(), 0);
        for (int i = 0, n = pts.size(); i < n; ++i) {
            order[i] = (i + n - m_point_offset) % n;
        }
    }
        break;
    }

    if (!order.empty())
    {
        if (m_point_reverse) {
            std::reverse(order.begin(), order.end());
        }
        attr.ChangePointsOrder(order);
    }
}

//////////////////////////////////////////////////////////////////////////
// class Sort::PointCmp
//////////////////////////////////////////////////////////////////////////

Sort::PointCmp::PointCmp(const std::vector<std::shared_ptr<GeoAttribute::Point>>& points, Key key)
    : m_points(points)
    , m_key(key)
{
}

bool Sort::PointCmp::operator () (size_t i0, size_t i1) const
{
    bool ret = false;
    switch (m_key)
    {
    case Key::X:
        ret = m_points[i0]->pos.x < m_points[i1]->pos.x;
        break;
    case Key::Y:
        ret = m_points[i0]->pos.y < m_points[i1]->pos.y;
        break;
    case Key::Z:
        ret = m_points[i0]->pos.z < m_points[i1]->pos.z;
        break;
    default:
        assert(0);
    }
    return ret;
}

}
}
