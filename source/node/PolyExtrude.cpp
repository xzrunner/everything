#include "everything/node/PolyExtrude.h"
#include "everything/Geometry.h"
#include "everything/GeoAttrHelper.h"

namespace evt
{
namespace node
{

void PolyExtrude::Execute(TreeContext& ctx)
{
    m_geo.reset();

    auto prev_geo = GetInputGeo(0);
    if (!prev_geo) {
        return;
    }

    m_geo = std::make_shared<Geometry>(*prev_geo);

    bool dirty = false;
    auto& prims = m_geo->GetAttr().GetPrimtives();
    auto group = m_geo->QueryGroup(m_group_name);
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
        m_geo->UpdateByAttr();
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
    auto normal = GeoAttrHelper::CalcFaceNormal(prim);
    auto offset = normal * m_distance;
    for (auto& v : prim.vertices) {
        v->point->pos += offset;
    }
}

}
}