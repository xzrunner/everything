#include "everything/GroupRebuild.h"
#include "everything/Group.h"

#include <map>

namespace evt
{

GroupRebuild::GroupRebuild(const GeoAttribute& attr, Group& group)
    : m_attr(attr)
    , m_group(group)
{
    switch (group.type)
    {
    case GroupType::Points:
    {
        m_points.reserve(group.items.size());
        auto& pts = attr.GetPoints();
        for (auto i : group.items) {
            assert(i < pts.size());
            m_points.push_back(pts[i]);
        }
    }
        break;
    case GroupType::Vertices:
    {
        m_vertices.reserve(group.items.size());
        auto& vts = attr.GetVertices();
        for (auto i : group.items) {
            assert(i < vts.size());
            m_vertices.push_back(vts[i]);
        }
    }
        break;
    case GroupType::Primitives:
    {
        m_primtives.reserve(group.items.size());
        auto& prims = attr.GetPrimtives();
        for (auto i : group.items) {
            assert(i < prims.size());
            m_primtives.push_back(prims[i]);
        }
    }
        break;
    default:
        assert(0);
    }
}

GroupRebuild::~GroupRebuild()
{
    switch (m_group.type)
    {
    case GroupType::Points:
    {
        auto& pts = m_attr.GetPoints();
        std::map<std::shared_ptr<GeoAttribute::Point>, size_t> p2idx;
        for (size_t i = 0, n = pts.size(); i < n; ++i) {
            p2idx.insert({ pts[i], i });
        }

        m_group.items.clear();
        m_group.items.reserve(m_points.size());
        for (auto& p : m_points)
        {
            assert(p);
            auto itr = p2idx.find(p);
            if (itr != p2idx.end()) {
                m_group.items.push_back(itr->second);
            }
        }
    }
        break;
    case GroupType::Vertices:
    {
        auto& vts = m_attr.GetVertices();
        std::map<std::shared_ptr<GeoAttribute::Vertex>, size_t> v2idx;
        for (size_t i = 0, n = vts.size(); i < n; ++i) {
            v2idx.insert({ vts[i], i });
        }

        m_group.items.clear();
        m_group.items.reserve(m_vertices.size());
        for (auto& v : m_vertices)
        {
            assert(v);
            auto itr = v2idx.find(v);
            if (itr != v2idx.end()) {
                m_group.items.push_back(itr->second);
            }
        }
    }
        break;
    case GroupType::Primitives:
    {
        auto& prims = m_attr.GetPrimtives();
        std::map<std::shared_ptr<GeoAttribute::Primitive>, size_t> prim2idx;
        for (size_t i = 0, n = prims.size(); i < n; ++i) {
            prim2idx.insert({ prims[i], i });
        }

        m_group.items.clear();
        m_group.items.reserve(m_primtives.size());
        for (auto& prim : m_primtives)
        {
            assert(prim);
            auto itr = prim2idx.find(prim);
            if (itr != prim2idx.end()) {
                m_group.items.push_back(itr->second);
            }
        }
    }
        break;
    default:
        assert(0);
    }
}

}