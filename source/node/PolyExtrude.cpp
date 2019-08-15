#include "everything/node/PolyExtrude.h"
#include "everything/Geometry.h"

#include <polymesh3/Geometry.h>

namespace evt
{
namespace node
{

void PolyExtrude::Execute(TreeContext& ctx)
{
    assert(m_imports.size() == 1);
    if (m_imports[0].conns.empty()) {
        return;
    }

    assert(m_imports[0].conns.size() == 1);
    auto prev = m_imports[0].conns[0].node.lock();
    if (!prev) {
        return;
    }

    auto prev_geo = prev->GetGeometry();
    if (!prev_geo) {
        m_geo.reset();
        return;
    }

    m_geo = std::make_shared<Geometry>(*prev_geo);

    m_geo->TraverseFaces([&](pm3::Polytope& poly, size_t face_idx)->bool
    {
        auto& faces = poly.Faces();
        assert(face_idx >= 0 && face_idx < faces.size());
        auto& face = faces[face_idx];
        assert(face);
        auto offset = face->plane.normal * m_distance;
        for (auto& v : face->points) {
            poly.Points()[v] += offset;
        }
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