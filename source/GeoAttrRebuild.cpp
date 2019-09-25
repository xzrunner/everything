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
    auto& attr = m_geo.GetAttr();
    for (int i = 0, n = static_cast<int>(GeoAttrType::MaxTypeNum); i < n; ++i) {
        m_var_descs[i] = attr.GetAttrDesc(static_cast<GeoAttrType>(i));
    }

    // points
    for (auto& p : attr.GetPoints()) {
        assert(!p->topo_id.Empty());
        m_points.insert({ p->topo_id.UID(), p->vars });
    }

    // vertices
    for (auto& v : attr.GetVertices())
    {
        auto& vert_id = v->point->topo_id;
        auto face = v->prim.lock();
        assert(face);
        auto& face_id = face->topo_id;
        assert(!vert_id.Empty() && !face_id.Empty());
        const uint64_t id = static_cast<uint64_t>(face_id.UID()) << 32 | vert_id.UID();
        m_vertices.insert({ id, v->vars });
    }

    // primitives
    for (auto& prim : attr.GetPrimtives()) {
        assert(!prim->topo_id.Empty());
        m_primitives.insert({ prim->topo_id.UID(), prim->vars });
    }

    // detail
    m_detail = attr.GetDetail().vars;
}

void GeoAttrRebuild::Load()
{
    auto& attr = m_geo.GetAttr();
    for (int i = 0, n = static_cast<int>(GeoAttrType::MaxTypeNum); i < n; ++i) {
        attr.SetAttrDesc(static_cast<GeoAttrType>(i), m_var_descs[i]);
    }

    // points
    auto default_attr_p = attr.GetDefaultValues(GeoAttrType::Point);
    for (auto& p : attr.GetPoints())
    {
        assert(!p->topo_id.Empty());
        auto itr = m_points.find(p->topo_id.UID());
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
        auto& vert_id = v->point->topo_id;
        auto face = v->prim.lock();
        assert(face);
        auto& face_id = face->topo_id;
        assert(!vert_id.Empty() && !face_id.Empty());
        const uint64_t id = static_cast<uint64_t>(face_id.UID()) << 32 | vert_id.UID();
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
        assert(!prim->topo_id.Empty());
        auto itr = m_primitives.find(prim->topo_id.UID());
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