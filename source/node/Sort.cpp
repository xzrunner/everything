#include "everything/node/Sort.h"
#include "everything/GeometryImpl.h"

namespace evt
{
namespace node
{

void Sort::Execute(Evaluator& eval, TreeContext& ctx)
{
    m_geo_impl.reset();

    auto prev_geo = GetInputGeo(0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    auto& attr = m_geo_impl->GetAttr();
    if (m_key == Key::NoChange)
    {
        attr.ResetPointsOrder();
    }
    else
    {
        auto sorted = attr.GetPoints();
        std::sort(sorted.begin(), sorted.end(), PointCmp(m_key));
        for (size_t i = 0, n = sorted.size(); i < n; ++i) {
            sorted[i]->order = i;
        }
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

Sort::PointCmp::PointCmp(Key key)
    : m_key(key)
{
}

bool Sort::PointCmp::operator () (const std::shared_ptr<GeoAttribute::Point>& i0,
                                  const std::shared_ptr<GeoAttribute::Point>& i1) const
{
    bool ret = false;
    switch (m_key)
    {
    case Key::X:
        ret = i0->pos.x < i1->pos.x;
        break;
    case Key::Y:
        ret = i0->pos.y < i1->pos.y;
        break;
    case Key::Z:
        ret = i0->pos.z < i1->pos.z;
        break;
    default:
        assert(0);
    }
    return ret;
}

}
}
