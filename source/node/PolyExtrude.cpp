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
    if (group)
    {
        assert(group->type == GroupType::Primitives);
        for (auto& i : group->items) {
            if (ExtrudeFace(*prims[i])) {
                dirty = true;
            }
        }
    }
    else
    {
        for (auto& p : prims) {
            if (ExtrudeFace(*p)) {
                dirty = true;
            }
        }
    }
    if (dirty) {
        m_geo_impl->UpdateByBrush(*brush_model);
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

bool PolyExtrude::ExtrudeFace(GeoAttribute::Primitive& prim) const
{
    auto brush_model = m_geo_impl->GetBrushModel();
    if (!brush_model) {
        return false;
    }

    auto& brushes = brush_model->GetBrushes();
    assert(prim.prim_id >= 0 && prim.prim_id < brushes.size());
    auto poly = brushes[prim.prim_id].impl;
    if (!poly) {
        return false;
    }
    auto he_poly = poly->GetGeometry();
    if (!he_poly) {
        return false;
    }

    if (he_poly->Extrude(m_distance, prim.topo_id,
        m_output_front, m_output_back, m_output_side)) {
        poly->BuildFromGeo();
        return true;
    } else {
        return false;
    }
}

}
}