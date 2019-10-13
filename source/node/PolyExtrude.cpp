#include "sop/node/PolyExtrude.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>

namespace sop
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
    auto brush_model = m_geo_impl->GetBrushModel();
    if (!brush_model) {
        return;
    }

    bool dirty = false;
    auto& prims = m_geo_impl->GetAttr().GetPrimtives();
    auto group = m_geo_impl->GetGroup().Query(m_group_name);
    std::vector<std::shared_ptr<GeoAttribute::Primitive>> edited_prims;
    if (group)
    {
        assert(group->type == GroupType::Primitives);
        for (auto& i : group->items) {
            edited_prims.push_back(prims[i]);
        }
    }
    else
    {
        edited_prims = prims;
    }

    if (ExtrudeFace(edited_prims)) {
        m_geo_impl->UpdateByBrush(*brush_model);
    }
}

void PolyExtrude::SetGroupName(const std::string& name)
{
    if (m_group_name == name) {
        return;
    }

    m_group_name = name;

    SetDirty(true);
}

void PolyExtrude::SetDistance(float dist)
{
    if (m_distance == dist) {
        return;
    }

    m_distance = dist;

    SetDirty(true);
}

void PolyExtrude::SetOutputFront(bool output)
{
    if (m_output_front == output) {
        return;
    }

    m_output_front = output;

    SetDirty(true);
}

void PolyExtrude::SetOutputBack(bool output)
{
    if (m_output_back == output) {
        return;
    }

    m_output_back = output;

    SetDirty(true);
}

void PolyExtrude::SetOutputSide(bool output)
{
    if (m_output_side == output) {
        return;
    }

    m_output_side = output;

    SetDirty(true);
}

void PolyExtrude::SetFrontGroupName(const std::string& name)
{
    if (m_front_group == name) {
        return;
    }

    m_front_group = name;

    SetDirty(true);
}

void PolyExtrude::SetBackGroupName(const std::string& name)
{
    if (m_back_group == name) {
        return;
    }

    m_back_group = name;

    SetDirty(true);
}

void PolyExtrude::SetSideGroupName(const std::string& name)
{
    if (m_side_group == name) {
        return;
    }

    m_side_group = name;

    SetDirty(true);
}

bool PolyExtrude::ExtrudeFace(const std::vector<std::shared_ptr<GeoAttribute::Primitive>>& prims) const
{
    auto brush_model = m_geo_impl->GetBrushModel();
    if (!brush_model) {
        return false;
    }

    auto& brushes = brush_model->GetBrushes();
    std::vector<std::vector<he::TopoID>> face_ids(brushes.size());
    for (auto& prim : prims) {
        assert(prim->prim_id >= 0 && prim->prim_id < brushes.size());
        face_ids[prim->prim_id].push_back(prim->topo_id);
    }

    bool dirty = false;

    for (int i = 0, n = brushes.size(); i < n; ++i)
    {
        if (face_ids[i].empty()) {
            continue;
        }

        auto poly = brushes[i].impl;
        if (!poly) {
            continue;
        }
        auto he_poly = poly->GetGeometry();
        if (!he_poly) {
            continue;
        }

        if (he_poly->Extrude(m_distance, face_ids[i],
            m_output_front, m_output_back, m_output_side)) {
            poly->BuildFromGeo();
            dirty = true;
        }
    }

    return dirty;
}

}
}