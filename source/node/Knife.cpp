#include "sop/node/Knife.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/GeoAttrRebuild.h"
#include "sop/GroupRebuild.h"

#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>

namespace sop
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
    if (!brush_model) {
        return;
    }
    assert(brush_model);

    GeoAttrRebuild attr_rebuild(*m_geo_impl, true);
    GroupRebuild group_rebuild(*m_geo_impl, true);

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

void Knife::SetOrigin(const sm::vec3& ori)
{
    if (m_props.SetValue(ORIGINX, Variable(ori))) {
        SetDirty(true);
    }
}

void Knife::SetDirection(const sm::vec3& dir)
{
    if (m_props.SetValue(DIR, Variable(dir))) {
        SetDirty(true);
    }
}

void Knife::SetKeepType(KeepType keep)
{
    NODE_PROP_SET(m_keep, keep);
}

void Knife::InitProps()
{
    m_props.Assign(ORIGINX, PropNames[ORIGINX], Variable(sm::vec3(0.0f, 0.0f, 0.0f)));
    m_props.Assign(DIR,     PropNames[DIR],     Variable(sm::vec3(0.0f, 1.0f, 0.0f)));
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

    auto& props = m_props.GetProps();

    assert(props[ORIGINX].Val().type == VarType::Float3);
    auto origin = static_cast<const sm::vec3*>(props[ORIGINX].Val().p);

    assert(props[DIR].Val().type == VarType::Float3);
    auto dir = static_cast<const sm::vec3*>(props[DIR].Val().p);

    sm::Plane plane(*dir, *origin);
    if (poly.GetGeometry()->Clip(plane, keep)) {
        poly.BuildFromGeo();
        return true;
    } else {
        return false;
    }
}

}
}