#include "everything/GroupMgr.h"

#include <set>

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
        if (itr == m_groups.end())
        {
            auto new_group = std::make_shared<Group>(*group.second);
            for (auto& i : new_group->items) {
                i += prim_off;
            }
            m_groups.insert({ new_group->name, new_group });
        }
        else
        {
            auto old_group = itr->second;
            for (auto& i : group.second->items) {
                old_group->items.push_back(i + prim_off);
            }
        }
    }
}

void GroupMgr::Add(const std::shared_ptr<Group>& group, GroupMerge merge_op)
{
    if (group->name.empty())
    {
        std::string name;
        do {
            name = "group" + std::to_string(m_next_id++);
        } while (m_groups.find(name) != m_groups.end());
        group->name = name;
    }

    auto itr = m_groups.insert({ group->name, group });
    if (!itr.second)
    {
        auto& old_group = itr.first->second;
        assert(group->name == old_group->name
            && group->type == old_group->type);
        Merge(merge_op, group->items, old_group->items);
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
    assert(itr != m_groups.end() && itr->second->name == src);
    auto group = itr->second;
    m_groups.erase(itr);
    group->name = dst;
    Add(group, GroupMerge::Replace);
}

void GroupMgr::Merge(GroupMerge op, const std::vector<size_t>& src, std::vector<size_t>& dst)
{
    switch (op)
    {
    case GroupMerge::Replace:
        dst = src;
        break;
    case GroupMerge::Union:
    {
        std::set<size_t> dst_set;
        for (auto& d : dst) {
            dst_set.insert(d);
        }
        for (auto& s : src) {
            if (dst_set.find(s) == dst_set.end()) {
                dst.push_back(s);
            }
        }
    }
        break;
    case GroupMerge::Intersect:
    {
        std::set<size_t> dst_set;
        for (auto& d : dst) {
            dst_set.insert(d);
        }
        dst.clear();
        for (auto& s : src) {
            if (dst_set.find(s) != dst_set.end()) {
                dst.push_back(s);
            }
        }
    }
        break;
    case GroupMerge::Subtract:
    {
        for (auto& s : src)
        {
            for (auto itr = dst.begin(); itr != dst.end(); )
            {
                if (s == *itr) {
                    itr = dst.erase(itr);
                } else {
                    ++itr;
                }
            }
        }
    }
        break;
    }
}

}