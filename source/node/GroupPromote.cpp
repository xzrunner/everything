#include "sop/node/GroupPromote.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

namespace
{

sop::GroupTypes trans_group_type(sop::GroupType type)
{
    switch (type)
    {
    case sop::GroupType::GuessFromGroup:
        return sop::GroupTypes::Auto;
    case sop::GroupType::Points:
        return sop::GroupTypes::Points;
    case sop::GroupType::Vertices:
        return sop::GroupTypes::Vertices;
    case sop::GroupType::Edges:
        return sop::GroupTypes::Edges;
    case sop::GroupType::Primitives:
        return sop::GroupTypes::Primitives;
    default:
        assert(0);
        return sop::GroupTypes::Auto;
    }
}

}

namespace sop
{
namespace node
{

void GroupPromote::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    assert(m_group_names.size() == m_src_types.size() &&
           m_src_types.size() == m_dst_types.size());
    for (size_t i = 0, n = m_group_names.size(); i < n; ++i)
    {
        if (m_group_names[i].empty() || m_src_types[i] == m_dst_types[i]) {
            continue;
        }

        auto group = m_geo_impl->GetGroup().Query(m_group_names[i]);
        if (!group) {
            continue;
        }
        if (m_src_types[i] != GroupTypes::Auto &&
            m_src_types[i] != trans_group_type(group->GetType())) {
            continue;
        }

        auto src_type = m_src_types[i];
        if (m_src_types[i] == GroupTypes::Auto) {
            src_type = trans_group_type(group->GetType());
        }
        if (src_type == GroupTypes::Primitives && m_dst_types[i] == GroupTypes::Points) {
            PrimsToPoints(*group);
        } else if (src_type == GroupTypes::Points && m_dst_types[i] == GroupTypes::Primitives) {
            PointsToPrims(*group);
        }
    }
}

void GroupPromote::PrimsToPoints(Group& group)
{
    assert(group.GetType() == GroupType::Primitives);

    auto& attr  = m_geo_impl->GetAttr();

    std::set<std::shared_ptr<GeoAttribute::Point>> selected;
    auto& prims = attr.GetPrimtives();
    for (auto& i : group.GetItems()) {
        assert(i >= 0 && i < prims.size());
        for (auto& v : prims[i]->vertices) {
            selected.insert(v->point);
        }
    }

    group.Clear();
    group.SetType(GroupType::Points);
    auto& pts = attr.GetPoints();
    std::vector<size_t> items;
    for (size_t i = 0, n = pts.size(); i < n; ++i) {
        if (selected.find(pts[i]) != selected.end()) {
            items.push_back(i);
        }
    }
    group.SetItems(items);
}

void GroupPromote::PointsToPrims(Group& group)
{
    assert(group.GetType() == GroupType::Points);

    auto& attr = m_geo_impl->GetAttr();

    std::set<std::shared_ptr<GeoAttribute::Point>> selected;
    auto& pts = attr.GetPoints();
    for (auto& i : group.GetItems()) {
        assert(i >= 0 && i < pts.size());
        selected.insert(pts[i]);
    }

    group.Clear();
    group.SetType(GroupType::Primitives);
    auto& prims = attr.GetPrimtives();
    std::vector<size_t> items;
    for (size_t i = 0, n = prims.size(); i < n; ++i)
    {
        bool find = false;
        for (auto& v : prims[i]->vertices) {
            if (selected.find(v->point) != selected.end()) {
                find = true;
                break;
            }
        }

        if (find) {
            items.push_back(i);
        }
    }
    group.SetItems(items);
}

}
}