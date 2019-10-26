#include "sop/node/GroupPromote.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

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

    if (m_group_name.empty() || m_src_type == m_dst_type) {
        return;
    }

    auto group = m_geo_impl->GetGroup().Query(m_group_name);
    if (!group) {
        return;
    }
    if (m_src_type != GroupType::GuessFromGroup &&
        m_src_type != group->GetType()) {
        return;
    }

    auto src_type = m_src_type;
    if (m_src_type == GroupType::GuessFromGroup) {
        src_type = group->GetType();
    }
    if (src_type == GroupType::Primitives && m_dst_type == GroupType::Points) {
        PrimsToPoints(*group);
    } else if (src_type == GroupType::Points && m_dst_type == GroupType::Primitives) {
        PointsToPrims(*group);
    }
}

void GroupPromote::SetGroupName(const std::string& name)
{
    NODE_PROP_SET(m_group_name, name);
}

void GroupPromote::SetSrcGroupType(GroupType type)
{
    NODE_PROP_SET(m_src_type, type);
}

void GroupPromote::SetDstGroupType(GroupType type)
{
    NODE_PROP_SET(m_dst_type, type);
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