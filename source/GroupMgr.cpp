#include "sop/GroupMgr.h"

#include <assert.h>

namespace sop
{

GroupMgr::GroupMgr(const GroupMgr& groups)
{
    operator = (groups);
}

GroupMgr& GroupMgr::operator = (const GroupMgr& groups)
{
    m_groups.clear();
    for (auto& itr : groups.m_groups) {
        m_groups.insert({ itr.first, std::make_shared<Group>(*itr.second) });
    }

    return *this;
}

void GroupMgr::Combine(const GroupMgr& groups, size_t pts_off,
                          size_t vts_off, size_t prim_off)
{
    for (auto& group : groups.m_groups)
    {
        size_t off = 0;
        switch (group.second->GetType())
        {
        case GroupType::Points:
            off = pts_off;
            break;
        case GroupType::Vertices:
            off = vts_off;
            break;
        case GroupType::Primitives:
            off = prim_off;
            break;
        default:
            assert(0);
        }

        auto itr = m_groups.find(group.second->GetName());
        if (itr == m_groups.end())
        {
            auto new_group = std::make_shared<Group>(*group.second);
            new_group->Offset(off);
            m_groups.insert({ new_group->GetName(), new_group });
        }
        else
        {
            auto old_group = itr->second;
            assert(old_group->GetType() == group.second->GetType());
            for (auto& i : group.second->GetItems()) {
                old_group->Add(i + off);
            }
        }
    }
}

void GroupMgr::Add(const std::shared_ptr<Group>& group, GroupMerge merge_op)
{
    if (group->GetName().empty())
    {
        std::string name;
        do {
            name = "group" + std::to_string(m_next_id++);
        } while (m_groups.find(name) != m_groups.end());
        group->SetName(name);
    }

    auto itr = m_groups.insert({ group->GetName(), group });
    if (!itr.second)
    {
        auto& old_group = itr.first->second;
        Group::Merge(merge_op, group->GetItems(),
            const_cast<std::vector<size_t>&>(old_group->GetItems()));
    }
}

std::shared_ptr<Group>
GroupMgr::Query(const std::string& name) const
{
    auto itr = m_groups.find(name);
    return itr == m_groups.end() ? nullptr : itr->second;
}

void GroupMgr::Rename(const std::string& src, const std::string& dst)
{
    auto itr = m_groups.find(src);
    assert(itr != m_groups.end() && itr->second->GetName() == src);
    auto group = itr->second;
    m_groups.erase(itr);
    group->SetName(dst);
    Add(group, GroupMerge::Replace);
}

void GroupMgr::Traverse(std::function<bool(const Group&)> func) const
{
    for (auto& pair : m_groups) {
        if (!func(*pair.second)) {
            break;
        }
    }
}

}