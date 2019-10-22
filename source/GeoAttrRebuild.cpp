#include "sop/GeoAttrRebuild.h"
#include "sop/GeometryImpl.h"

namespace
{

std::map<uint64_t, std::vector<sop::VarValue>>::const_iterator
QueryFromCache(const std::map<uint64_t, std::vector<sop::VarValue>>& cache, const he::TopoID& id, bool split)
{
    auto itr = cache.find(id.UID());
    if (itr != cache.end()) {
        return itr;
    }

    if (split && id.Path().size() > 1)
    {
        he::TopoID pid(id);
        pid.Pop();
        itr = cache.find(pid.UID());
        if (itr != cache.end()) {
            return itr;
        }
    }

    return cache.end();
}

}

namespace sop
{

GeoAttrRebuild::GeoAttrRebuild(GeometryImpl& geo, bool prim_split)
    : m_geo(geo)
    , m_prim_split(prim_split)
{
    Save(m_dump, m_geo);
}

GeoAttrRebuild::~GeoAttrRebuild()
{
    Load(m_geo, m_dump, m_prim_split);
}

void GeoAttrRebuild::Copy(GeometryImpl& dst, GeometryImpl& src, bool prim_split)
{
    AttrDump tmp;
    Save(tmp, src);
    Load(dst, tmp, prim_split);
}

void GeoAttrRebuild::Save(AttrDump& dst, const GeometryImpl& src)
{
    auto& attr = src.GetAttr();
    for (int i = 0, n = static_cast<int>(GeoAttrClass::MaxTypeNum); i < n; ++i) {
        dst.var_descs[i] = attr.GetAttrDesc(static_cast<GeoAttrClass>(i));
    }

    // points
    for (auto& p : attr.GetPoints()) {
        assert(!p->topo_id.Empty());
        dst.points.insert({ p->topo_id.UID(), p->vars });
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
        dst.vertices.insert({ id, v->vars });
    }

    // primitives
    for (auto& prim : attr.GetPrimtives()) {
        assert(!prim->topo_id.Empty());
        dst.primitives.insert({ prim->topo_id.UID(), prim->vars });
    }

    // detail
    dst.detail = attr.GetDetail().vars;
}

void GeoAttrRebuild::Load(GeometryImpl& dst, const AttrDump& src, bool prim_split)
{
    auto& attr = dst.GetAttr();
    for (int i = 0, n = static_cast<int>(GeoAttrClass::MaxTypeNum); i < n; ++i) {
        attr.SetAttrDesc(static_cast<GeoAttrClass>(i), src.var_descs[i]);
    }

    // points
    auto default_attr_p = attr.GetDefaultValues(GeoAttrClass::Point);
    for (auto& p : attr.GetPoints())
    {
        assert(!p->topo_id.Empty());
        auto itr = QueryFromCache(src.points, p->topo_id, prim_split);
        if (itr != src.points.end()) {
            p->vars = itr->second;
        } else {
            p->vars = default_attr_p;
        }
    }

    // vertices
    auto default_attr_v = attr.GetDefaultValues(GeoAttrClass::Vertex);
    for (auto& v : attr.GetVertices())
    {
        auto& vert_id = v->point->topo_id;
        auto face = v->prim.lock();
        assert(face);
        auto& face_id = face->topo_id;
        assert(!vert_id.Empty() && !face_id.Empty());
        const uint64_t id = static_cast<uint64_t>(face_id.UID()) << 32 | vert_id.UID();
        auto itr = src.vertices.find(id);
        if (itr != src.vertices.end()) {
            v->vars = itr->second;
        } else {
            v->vars = default_attr_v;
        }
    }

    // primitives
    auto default_attr_prim = attr.GetDefaultValues(GeoAttrClass::Primitive);
    for (auto& prim : attr.GetPrimtives())
    {
        assert(!prim->topo_id.Empty());
        auto itr = QueryFromCache(src.primitives, prim->topo_id, prim_split);
        if (itr != src.primitives.end()) {
            prim->vars = itr->second;
        } else {
            prim->vars = default_attr_prim;
        }
    }

    // detail
    const_cast<GeoAttribute::Detail&>(attr.GetDetail()).vars = src.detail;
}

}