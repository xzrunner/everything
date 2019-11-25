#include "sop/GeoAttrRebuild.h"
#include "sop/GeometryImpl.h"
#include "sop/ParmList.h"

namespace
{

std::map<uint64_t, uint32_t>::const_iterator
QueryFromCache(const std::map<uint64_t, uint32_t>& cache, const he::TopoID& id, bool split)
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

    auto& src_lists = attr.GetAllParmLists();
    for (size_t i = 0; i < AttrDump::MAX_LSIT_COUNT; ++i) {
        dst.parm_lists[i].resize(src_lists[i].size());
        for (size_t j = 0, m = src_lists[i].size(); j < m; ++j) {
            dst.parm_lists[i][j] = src_lists[i][j]->Clone();
        }
    }

    // points
    for (auto& p : attr.GetPoints()) {
        assert(!p->topo_id.Empty());
        dst.points.insert({ p->topo_id.UID(), p->attr_idx });
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
        dst.vertices.insert({ id, v->attr_idx });
    }

    // primitives
    for (auto& prim : attr.GetPrimtives()) {
        assert(!prim->topo_id.Empty());
        dst.primitives.insert({ prim->topo_id.UID(), prim->attr_idx });
    }
}

void GeoAttrRebuild::Load(GeometryImpl& dst, const AttrDump& src, bool prim_split)
{
    auto& attr = dst.GetAttr();

    // points
    std::vector<std::shared_ptr<ParmList>> p_lists;
    auto& pts = attr.GetPoints();
    auto& p_src = src.parm_lists[static_cast<size_t>(GeoAttrClass::Point)];
    p_lists.reserve(p_src.size());
    for (auto& s : p_src)
    {
        auto d = s->Clone(false);
        d->Resize(pts.size());
        p_lists.push_back(d);
    }
    for (size_t i = 0, n = pts.size(); i < n; ++i)
    {
        assert(!pts[i]->topo_id.Empty());
        auto itr = QueryFromCache(src.points, pts[i]->topo_id, prim_split);
        if (itr != src.points.end()) {
            for (size_t j = 0, m = p_src.size(); j < m; ++j) {
                p_lists[j]->CopyFrom(i, *p_src[j], itr->second);
            }
        }
    }
    attr.SetParmLists(GeoAttrClass::Point, p_lists);

    // vertices

    std::vector<std::shared_ptr<ParmList>> v_lists;
    auto& vts = attr.GetVertices();
    auto& v_src = src.parm_lists[static_cast<size_t>(GeoAttrClass::Vertex)];
    v_lists.reserve(v_src.size());
    for (auto& s : v_src)
    {
        auto d = s->Clone(false);
        d->Resize(vts.size());
        v_lists.push_back(d);
    }
    for (size_t i = 0, n = vts.size(); i < n; ++i)
    {
        auto& vert_id = vts[i]->point->topo_id;
        auto face = vts[i]->prim.lock();
        assert(face);
        auto& face_id = face->topo_id;
        assert(!vert_id.Empty() && !face_id.Empty());
        const uint64_t id = static_cast<uint64_t>(face_id.UID()) << 32 | vert_id.UID();
        auto itr = src.vertices.find(id);
        if (itr != src.vertices.end()) {
            for (size_t j = 0, m = v_src.size(); j < m; ++j) {
                v_lists[j]->CopyFrom(i, *v_src[j], itr->second);
            }
        }
    }
    attr.SetParmLists(GeoAttrClass::Vertex, v_lists);


    // primitives
    std::vector<std::shared_ptr<ParmList>> prim_lists;
    auto& prims = attr.GetPrimtives();
    auto& prim_src = src.parm_lists[static_cast<size_t>(GeoAttrClass::Primitive)];
    prim_lists.reserve(prim_src.size());
    for (auto& s : prim_src)
    {
        auto d = s->Clone(false);
        d->Resize(prims.size());
        prim_lists.push_back(d);
    }
    for (size_t i = 0, n = prims.size(); i < n; ++i)
    {
        assert(!prims[i]->topo_id.Empty());
        auto itr = QueryFromCache(src.primitives, prims[i]->topo_id, prim_split);
        if (itr != src.primitives.end()) {
            for (size_t j = 0, m = prim_src.size(); j < m; ++j) {
                prim_lists[j]->CopyFrom(i, *prim_src[j], itr->second);
            }
        }
    }
    attr.SetParmLists(GeoAttrClass::Primitive, prim_lists);

    // detail
    attr.SetParmLists(GeoAttrClass::Detail, src.parm_lists[static_cast<size_t>(GeoAttrClass::Detail)]);
}

}