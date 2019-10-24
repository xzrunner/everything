#include "sop/node/Sort.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

namespace sop
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
    auto& pts = attr.GetPoints();

    switch (m_key)
    {
    case Key::X:
    case Key::Y:
    case Key::Z:
    {
        std::vector<size_t> order;
        order.resize(pts.size());
        for (size_t i = 0, n = pts.size(); i < n; ++i) {
            order[i] = i;
        }
        std::sort(order.begin(), order.end(), PointCmp(pts, m_key));
        attr.ChangePointsOrder(order);
    }
        break;
    case Key::Shift:
    {
        assert(m_props.GetProps()[POINT_OFFSET].Val().type == VarType::Int);
        const int p_off = m_props.GetProps()[POINT_OFFSET].Val().i;
        std::vector<size_t> order(pts.size(), 0);
        for (int i = 0, n = pts.size(); i < n; ++i) {
            order[i] = (i + n - p_off) % n;
        }
        attr.ChangePointsOrder(order);
    }
        break;
    }
}

void Sort::SetKey(Key key)
{
    NODE_PROP_SET(m_key, key);
}

void Sort::SetPointOffset(int off)
{
    bool dirty = false;

    if (m_props.SetValue(POINT_OFFSET, Variable(off))) {
        dirty = true;
    }

    if (dirty) {
        SetDirty(true);
    }
}

void Sort::InitProps()
{
    m_props.Assign(POINT_OFFSET, PropNames[POINT_OFFSET], Variable(0));
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
