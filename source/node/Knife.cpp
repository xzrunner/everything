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
    bool dirty = false;

    if (m_props.SetValue(ORIGINX_X, Variable(ori.x))) {
        dirty = true;
    }
    if (m_props.SetValue(ORIGINX_Y, Variable(ori.y))) {
        dirty = true;
    }
    if (m_props.SetValue(ORIGINX_Z, Variable(ori.z))) {
        dirty = true;
    }

    if (dirty) {
        SetDirty(true);
    }
}

void Knife::SetDirection(const sm::vec3& dir)
{
    bool dirty = false;

    if (m_props.SetValue(DIR_X, Variable(dir.x))) {
        dirty = true;
    }
    if (m_props.SetValue(DIR_Y, Variable(dir.y))) {
        dirty = true;
    }
    if (m_props.SetValue(DIR_Z, Variable(dir.z))) {
        dirty = true;
    }

    if (dirty) {
        SetDirty(true);
    }
}

void Knife::SetKeepType(KeepType keep)
{
    NODE_PROP_SET(m_keep, keep);
}

void Knife::InitProps()
{
    m_props.Assign(ORIGINX_X, PropNames[ORIGINX_X], Variable(0.0f));
    m_props.Assign(ORIGINX_Y, PropNames[ORIGINX_Y], Variable(0.0f));
    m_props.Assign(ORIGINX_Z, PropNames[ORIGINX_Z], Variable(0.0f));

    m_props.Assign(DIR_X, PropNames[DIR_X], Variable(0.0f));
    m_props.Assign(DIR_Y, PropNames[DIR_Y], Variable(1.0f));
    m_props.Assign(DIR_Z, PropNames[DIR_Z], Variable(0.0f));
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

    assert(NodeHelper::CheckPropsType(*this, 0, MAX_BUILD_IN_PROP, VarType::Float));
    auto& props = m_props.GetProps();

    auto origin = sm::vec3(
        props[ORIGINX_X].Val().f,
        props[ORIGINX_Y].Val().f,
        props[ORIGINX_Z].Val().f
    );

    auto dir = sm::vec3(
        props[DIR_X].Val().f,
        props[DIR_Y].Val().f,
        props[DIR_Z].Val().f
    );

    sm::Plane plane(dir, origin);
    if (poly.GetGeometry()->Clip(plane, keep)) {
        poly.BuildFromGeo();
        return true;
    } else {
        return false;
    }
}

}
}