#include "everything/node/PolyExtrude.h"
#include "everything/Geometry.h"

#include <polymesh3/Geometry.h>

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

    m_geo->TraverseFaces([&](pm3::Polytope& poly, size_t face_idx, bool& dirty)->bool
    {
        auto& faces = poly.Faces();
        assert(face_idx >= 0 && face_idx < faces.size());
        auto& face = faces[face_idx];
        assert(face);
        auto offset = face->plane.normal * m_distance;
        for (auto& v : face->points) {
            poly.Points()[v] += offset;
        }
        dirty = true;
        return true;
    }, m_group_name);
}

void PolyExtrude::SetDistance(float dist)
{
    if (m_distance == dist) {
        return;
    }

    m_distance = dist;

    SetDirty(true);
}

}
}