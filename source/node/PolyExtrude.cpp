#include "everything/node/PolyExtrude.h"
#include "everything/GeometryImpl.h"
#include "everything/NodeHelper.h"

namespace evt
{
namespace node
{

void PolyExtrude::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    bool dirty = false;
    auto& prims = m_geo_impl->GetAttr().GetPrimtives();
    auto group = m_geo_impl->GetGroup().Query(m_group_name);
    if (group)
    {
        assert(group->type == GroupType::Primitives);
        for (auto& i : group->items) {
            ExtrudeFace(*prims[i]);
            dirty = true;
        }
    }
    else
    {
        for (auto& p : prims) {
            ExtrudeFace(*p);
            dirty = true;
        }
    }
    if (dirty) {
        m_geo_impl->UpdateByAttr();
    }
}

void PolyExtrude::SetDistance(float dist)
{
    if (m_distance == dist) {
        return;
    }

    m_distance = dist;

    SetDirty(true);
}

void PolyExtrude::ExtrudeFace(GeoAttribute::Primitive& prim) const
{
    auto normal = prim.CalcNormal();
    auto offset = normal * m_distance;
    for (auto& v : prim.vertices) {
        v->point->pos += offset;
    }
}

}
}