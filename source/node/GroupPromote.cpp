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
    if (src_type == GroupType::Primitives &&
        m_dst_type == GroupType::Points) {
        PrimsToPoints(*group);
    }
}

void GroupPromote::SetGroupName(const std::string& name)
{
    if (m_group_name == name) {
        return;
    }

    m_group_name = name;

    SetDirty(true);
}

void GroupPromote::SetSrcGroupType(GroupType type)
{
    if (m_src_type == type) {
        return;
    }

    m_src_type = type;

    SetDirty(true);
}

void GroupPromote::SetDstGroupType(GroupType type)
{
    if (m_dst_type == type) {
        return;
    }

    m_dst_type = type;

    SetDirty(true);
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

}
}