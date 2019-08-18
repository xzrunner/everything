#include "everything/GroupMgr.h"

#include <assert.h>

namespace evt
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

void GroupMgr::Combine(const GroupMgr& groups, size_t prim_off)
{
    for (auto& group : groups.m_groups)
    {
        auto itr = m_groups.find(group.second->name);
        assert(itr == m_groups.end());
        assert(group.second->type == GroupType::Primitives);
        auto new_group = std::make_shared<Group>(*group.second);
        for (auto& i : new_group->items) {
            i += prim_off;
        }
        m_groups.insert({ new_group->name, new_group });
    }
}

void GroupMgr::Add(const std::shared_ptr<Group>& group)
{
    auto itr = m_groups.insert({ group->name, group });
    assert(itr.second);
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
    assert(itr != m_groups.end() && itr->second->name == src);
    auto group = itr->second;
    m_groups.erase(itr);
    group->name = dst;
    Add(group);
}

}