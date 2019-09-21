#include "everything/GeoAttrRebuild.h"
#include "everything/GeometryImpl.h"

#include <model/BrushModel.h>
#include <polymesh3/Geometry.h>
#include <halfedge/Polyhedron.h>

namespace evt
{

GeoAttrRebuild::GeoAttrRebuild(GeometryImpl& geo)
    : m_geo(geo)
{
    Save();
}

GeoAttrRebuild::~GeoAttrRebuild()
{
    Load();
}

void GeoAttrRebuild::Save()
{
    if (m_geo.GetAdaptorType() != GeoAdaptor::Type::Brush) {
        return;
    }

    auto& attr = m_geo.GetAttr();
    for (int i = 0, n = static_cast<int>(GeoAttrType::MaxTypeNum); i < n; ++i) {
        m_var_descs[i] = attr.GetAttrDesc(static_cast<GeoAttrType>(i));
    }

    auto brush_model = m_geo.GetBrushModel();
    auto& brushes = brush_model->GetBrushes();
    if (brushes.empty()) {
        return;
    }
    assert(brushes.size() == 1);
    auto& brush = brushes.front();
    auto he_poly = brush.impl->GetGeometry();

    // points
    auto& pts = attr.GetPoints();
    auto& he_vts = he_poly->GetVertices();
    assert(pts.size() == he_vts.Size());
    size_t idx_p = 0;
    auto first_p = he_vts.Head();
    auto curr_p = first_p;
    do {
        assert(pts[idx_p]->pos == curr_p->position);
        m_points.insert({ curr_p->id, pts[idx_p++]->vars });

        curr_p = curr_p->linked_next;
    } while (curr_p != first_p);

    // vertices
    auto& vts = attr.GetVertices();
    size_t idx_v = 0;
    auto& he_faces = he_poly->GetFaces();
    auto first_face = he_faces.Head();
    auto curr_face = first_face;
    do {
        auto first_edge = curr_face->edge;
        auto curr_edge = first_edge;
        std::shared_ptr<GeoAttribute::Primitive> prim = nullptr;
        do {
            int id = (curr_face->id) << 16 | curr_edge->vert->id;
            assert(idx_v < vts.size());
            if (!prim) {
                prim = vts[idx_v]->prim.lock();
            } else {
                assert(vts[idx_v]->prim.lock() == prim);
            }
            m_vertices.insert({ id, vts[idx_v++]->vars });

            curr_edge = curr_edge->next;
        } while (curr_edge != first_edge);

        curr_face = curr_face->linked_next;
    } while (curr_face != first_face);
    assert(idx_v == vts.size());

    // primitives
    auto& prims = attr.GetPrimtives();
    assert(prims.size() == he_faces.Size());
    size_t idx_prim = 0;
    first_face = he_faces.Head();
    curr_face = first_face;
    do {
        m_primitives.insert({ curr_face->id, prims[idx_prim++]->vars });

        curr_face = curr_face->linked_next;
    } while (curr_face != first_face);

    // detail
    m_detail = attr.GetDetail().vars;
}

void GeoAttrRebuild::Load()
{
    if (m_geo.GetAdaptorType() != GeoAdaptor::Type::Brush) {
        return;
    }

    auto& attr = m_geo.GetAttr();
    for (int i = 0, n = static_cast<int>(GeoAttrType::MaxTypeNum); i < n; ++i) {
        attr.SetAttrDesc(static_cast<GeoAttrType>(i), m_var_descs[i]);
    }

    auto brush_model = m_geo.GetBrushModel();
    auto& brushes = brush_model->GetBrushes();
    if (brushes.empty()) {
        return;
    }
    assert(brushes.size() == 1);
    auto& brush = brushes.front();
    auto he_poly = brush.impl->GetGeometry();

    // points
    auto& pts = attr.GetPoints();
    auto& he_vts = he_poly->GetVertices();
    assert(pts.size() == he_vts.Size());
    size_t idx_p = 0;
    auto first_p = he_vts.Head();
    auto curr_p = first_p;
    auto default_attr_p = attr.GetDefaultValues(GeoAttrType::Point);
    do {
        assert(pts[idx_p]->pos == curr_p->position);
        auto itr = m_points.find(curr_p->id);
        if (itr != m_points.end()) {
            pts[idx_p++]->vars = itr->second;
        } else {
            pts[idx_p++]->vars = default_attr_p;
        }

        curr_p = curr_p->linked_next;
    } while (curr_p != first_p);

    // vertices
    auto& vts = attr.GetVertices();
    size_t idx_v = 0;
    auto& he_faces = he_poly->GetFaces();
    auto first_face = he_faces.Head();
    auto curr_face = first_face;
    auto default_attr_v = attr.GetDefaultValues(GeoAttrType::Vertex);
    do {
        auto first_edge = curr_face->edge;
        auto curr_edge = first_edge;
        std::shared_ptr<GeoAttribute::Primitive> prim = nullptr;
        do {
            int id = (curr_face->id) << 16 | curr_edge->vert->id;
            assert(idx_v < vts.size());
            auto itr = m_vertices.find(id);
            if (itr != m_vertices.end()) {
                vts[idx_v++]->vars = itr->second;
            } else {
                vts[idx_v++]->vars = default_attr_v;
            }

            curr_edge = curr_edge->next;
        } while (curr_edge != first_edge);

        curr_face = curr_face->linked_next;
    } while (curr_face != first_face);
    assert(idx_v == vts.size());

    // primitives
    auto& prims = attr.GetPrimtives();
    assert(prims.size() == he_faces.Size());
    size_t idx_prim = 0;
    first_face = he_faces.Head();
    curr_face = first_face;
    auto default_attr_prim = attr.GetDefaultValues(GeoAttrType::Primitive);
    do {
        auto itr = m_primitives.find(curr_face->id);
        if (itr != m_primitives.end()) {
            prims[idx_prim++]->vars = itr->second;
        } else {
            prims[idx_prim++]->vars = default_attr_prim;
        }

        curr_face = curr_face->linked_next;
    } while (curr_face != first_face);

    // detail
    const_cast<GeoAttribute::Detail&>(attr.GetDetail()).vars = m_detail;
}

}