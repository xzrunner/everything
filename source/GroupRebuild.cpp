#include "sop/GroupRebuild.h"
#include "sop/Group.h"
#include "sop/GeometryImpl.h"

namespace sop
{

GroupRebuild::GroupRebuild(GeometryImpl& geo, bool prim_split)
    : m_geo(geo)
    , m_prim_split(prim_split)
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
        switch (group.GetType())
        {
        case GroupType::Points:
        {
            auto& pts = m_geo.GetAttr().GetPoints();
            dump.points.reserve(group.Size());
            for (auto i : group.GetItems())
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
            dump.vertices.reserve(group.Size());
            for (auto i : group.GetItems())
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
            for (auto i : group.GetItems())
            {
                auto& prim = prims[i];
                assert(!prim->topo_id.Empty());
                dump.primtives.insert(prim->topo_id.UID());
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

    auto itr = m_groups.begin();
    m_geo.GetGroup().Traverse([&](const Group& group)->bool
    {
        assert(itr != m_groups.end() && itr->ori == &group);

        auto& dst = const_cast<Group&>(group);

        switch (group.GetType())
        {
        case GroupType::Points:
        {
            std::vector<size_t> items;
            items.reserve(itr->points.size());
            for (auto& p : itr->points)
            {
                auto itr = p_topo2idx.find(p);
                if (itr != p_topo2idx.end()) {
                    items.push_back(itr->second);
                }
            }
            dst.SetItems(items);
        }
            break;
        case GroupType::Vertices:
        {
            std::vector<size_t> items;
            items.reserve(itr->vertices.size());
            for (auto& v : itr->vertices)
            {
                auto itr = v_topo2idx.find(v);
                if (itr != v_topo2idx.end()) {
                    items.push_back(itr->second);
                }
            }
            dst.SetItems(items);
        }
            break;
        case GroupType::Primitives:
        {
            auto& prims = m_geo.GetAttr().GetPrimtives();
            std::vector<size_t> items;
            for (size_t i = 0, n = prims.size(); i < n; ++i)
            {
                auto& prim = prims[i];
                if (itr->primtives.find(prim->topo_id.UID()) != itr->primtives.end()) {
                    items.push_back(i);
                }
                if (m_prim_split)
                {
                    he::TopoID pid = prim->topo_id;
                    pid.Pop();
                    if (itr->primtives.find(pid.UID()) != itr->primtives.end()) {
                        items.push_back(i);
                    }
                }
            }
            dst.SetItems(items);
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