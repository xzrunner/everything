#include "everything/GeoAttrRebuild.h"
#include "everything/GeometryImpl.h"

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

    // points
    for (auto& p : attr.GetPoints()) {
        assert(p->topo_id >= 0);
        m_points.insert({ p->topo_id, p->vars });
    }

    // vertices
    for (auto& v : attr.GetVertices())
    {
        int vert_id = v->point->topo_id;
        auto face = v->prim.lock();
        assert(face);
        int face_id = face->topo_id;
        assert(vert_id >= 0 && face_id >= 0);
        int id = face_id << 16 | vert_id;
        m_vertices.insert({ id, v->vars });
    }

    // primitives
    for (auto& prim : attr.GetPrimtives()) {
        assert(prim->topo_id >= 0);
        m_primitives.insert({ prim->topo_id, prim->vars });
    }

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

    // points
    auto default_attr_p = attr.GetDefaultValues(GeoAttrType::Point);
    for (auto& p : attr.GetPoints())
    {
        assert(p->topo_id >= 0);
        auto itr = m_points.find(p->topo_id);
        if (itr != m_points.end()) {
            p->vars = itr->second;
        } else {
            p->vars = default_attr_p;
        }
    }

    // vertices
    auto default_attr_v = attr.GetDefaultValues(GeoAttrType::Vertex);
    for (auto& v : attr.GetVertices())
    {
        int vert_id = v->point->topo_id;
        auto face = v->prim.lock();
        assert(face);
        int face_id = face->topo_id;
        assert(vert_id >= 0 && face_id >= 0);
        int id = face_id << 16 | vert_id;
        auto itr = m_vertices.find(id);
        if (itr != m_vertices.end()) {
            v->vars = itr->second;
        } else {
            v->vars = default_attr_v;
        }
    }

    // primitives
    auto default_attr_prim = attr.GetDefaultValues(GeoAttrType::Primitive);
    for (auto& prim : attr.GetPrimtives())
    {
        assert(prim->topo_id >= 0);
        auto itr = m_primitives.find(prim->topo_id);
        if (itr != m_primitives.end()) {
            prim->vars = itr->second;
        } else {
            prim->vars = default_attr_prim;
        }
    }

    // detail
    const_cast<GeoAttribute::Detail&>(attr.GetDetail()).vars = m_detail;
}

}