#include "everything/node/Knife.h"
#include "everything/GeometryImpl.h"
#include "everything/NodeHelper.h"
#include "everything/GeoAttrRebuild.h"

#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>

namespace evt
{
namespace node
{

void Knife::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    auto brush_model = m_geo_impl->GetBrushModel();
    assert(brush_model);

    GeoAttrRebuild attr_rebuild(*m_geo_impl);

    bool dirty = false;
    for (auto& brush : brush_model->GetBrushes()) {
        if (Clip(*brush.impl)) {
            dirty = true;
        }
    }

    if (dirty) {
        m_geo_impl->UpdateByBrush(*brush_model);
    }
}

void Knife::SetOrigin(const sm::vec3& origin)
{
    if (m_origin == origin) {
        return;
    }

    m_origin = origin;

    SetDirty(true);
}

void Knife::SetDirection(const sm::vec3& dir)
{
    if (m_direction == dir) {
        return;
    }

    m_direction = dir;

    SetDirty(true);
}

void Knife::SetKeepType(KeepType keep)
{
    if (m_keep == keep) {
        return;
    }

    m_keep = keep;

    SetDirty(true);
}

bool Knife::Clip(pm3::Polytope& poly) const
{
    he::Polyhedron::KeepType keep;
    switch (m_keep)
    {
    case KeepType::KeepAbove:
        keep = he::Polyhedron::KeepType::KeepAbove;
        break;
    case KeepType::KeepBelow:
        keep = he::Polyhedron::KeepType::KeepBelow;
        break;
    case KeepType::KeepAll:
        keep = he::Polyhedron::KeepType::KeepAll;
        break;
    default:
        assert(0);
    }

    sm::Plane plane(m_direction, m_origin);
    if (poly.GetGeometry()->Clip(plane, keep)) {
        poly.BuildFromGeo();
        return true;
    } else {
        return false;
    }
}

}
}