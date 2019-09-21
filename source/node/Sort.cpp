#include "everything/node/Sort.h"
#include "everything/GeometryImpl.h"
#include "everything/NodeHelper.h"

namespace evt
{
namespace node
{

void Sort::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    auto& attr = m_geo_impl->GetAttr();
    if (m_key != Key::NoChange)
    {
        std::vector<size_t> order;
        order.resize(attr.GetPoints().size());
        for (size_t i = 0, n = attr.GetPoints().size(); i < n; ++i) {
            order[i] = i;
        }
        std::sort(order.begin(), order.end(), PointCmp(attr.GetPoints(), m_key));
        attr.ChangePointsOrder(order);
    }
}

void Sort::SetKey(Key key)
{
    if (m_key == key) {
        return;
    }

    m_key = key;

    SetDirty(true);
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
