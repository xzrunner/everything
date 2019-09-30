#include "sop/GroupRebuild.h"
#include "sop/Group.h"
#include "sop/GeometryImpl.h"

namespace sop
{

GroupRebuild::GroupRebuild(GeometryImpl& geo)
    : m_geo(geo)
{
    Save();
}

GroupRebuild::~GroupRebuild()
{
    Load();
}

void GroupRebuild::Save()
{
    m_geo.GetGroup().Traverse([&](const Group& group)->bool
    {
        GroupDump dump;
        dump.ori = &group;
        switch (group.type)
        {
        case GroupType::Points:
        {
            auto& pts = m_geo.GetAttr().GetPoints();
            dump.points.reserve(group.items.size());
            for (auto i : group.items)
            {
                auto& p = pts[i];
                assert(!p->topo_id.Empty());
                dump.points.push_back(p->topo_id.UID());
            }
        }
            break;
        case GroupType::Vertices:
        {
            auto& vts = m_geo.GetAttr().GetVertices();
            dump.vertices.reserve(group.items.size());
            for (auto i : group.items)
            {
                auto& v = vts[i];
                auto& vert_id = v->point->topo_id;
                auto face = v->prim.lock();
                assert(face);
                auto& face_id = face->topo_id;
                assert(!vert_id.Empty() && !face_id.Empty());
                const uint64_t id = static_cast<uint64_t>(face_id.UID()) << 32 | vert_id.UID();
                dump.vertices.push_back(id);
            }
        }
            break;
        case GroupType::Primitives:
        {
            auto& prims = m_geo.GetAttr().GetPrimtives();
            dump.primtives.reserve(group.items.size());
            for (auto i : group.items)
            {
                auto& prim = prims[i];
                assert(!prim->topo_id.Empty());
                dump.primtives.push_back(prim->topo_id.UID());
            }
        }
            break;
        default:
            assert(0);
        }
        m_groups.push_back(dump);

        return true;
    });
}

void GroupRebuild::Load()
{
    if (m_groups.empty()) {
        return;
    }

    std::map<uint64_t, size_t> p_topo2idx;
    auto& pts = m_geo.GetAttr().GetPoints();
    for (size_t i = 0, n = pts.size(); i < n; ++i) {
        assert(!pts[i]->topo_id.Empty());
        p_topo2idx.insert({ pts[i]->topo_id.UID(), i });
    }

    std::map<uint64_t, size_t> v_topo2idx;
    auto& vts = m_geo.GetAttr().GetVertices();
    for (size_t i = 0, n = vts.size(); i < n; ++i)
    {
        auto& v = vts[i];
        auto& vert_id = v->point->topo_id;
        auto face = v->prim.lock();
        assert(face);
        auto& face_id = face->topo_id;
        assert(!vert_id.Empty() && !face_id.Empty());
        v_topo2idx.insert({ static_cast<uint64_t>(face_id.UID()) << 32 | vert_id.UID(), i });
    }

    std::map<uint64_t, size_t> prim_topo2idx;
    auto& prims = m_geo.GetAttr().GetPrimtives();
    for (size_t i = 0, n = prims.size(); i < n; ++i) {
        assert(!prims[i]->topo_id.Empty());
        prim_topo2idx.insert({ prims[i]->topo_id.UID(), i });
    }

    auto itr = m_groups.begin();
    m_geo.GetGroup().Traverse([&](const Group& group)->bool
    {
        assert(itr != m_groups.end() && itr->ori == &group);

        auto& dst = const_cast<Group&>(group);

        switch (group.type)
        {
        case GroupType::Points:
        {
            dst.items.clear();
            dst.items.reserve(itr->points.size());
            for (auto& p : itr->points)
            {
                auto itr = p_topo2idx.find(p);
                if (itr != p_topo2idx.end()) {
                    dst.items.push_back(itr->second);
                }
            }
        }
            break;
        case GroupType::Vertices:
        {
            dst.items.clear();
            dst.items.reserve(itr->vertices.size());
            for (auto& v : itr->vertices)
            {
                auto itr = v_topo2idx.find(v);
                if (itr != v_topo2idx.end()) {
                    dst.items.push_back(itr->second);
                }
            }
        }
            break;
        case GroupType::Primitives:
        {
            dst.items.clear();
            dst.items.reserve(itr->primtives.size());
            for (auto& prim : itr->primtives)
            {
                auto itr = prim_topo2idx.find(prim);
                if (itr != prim_topo2idx.end()) {
                    dst.items.push_back(itr->second);
                }
            }
        }
            break;
        default:
            assert(0);
        }

        ++itr;

        return true;
    });
}

}