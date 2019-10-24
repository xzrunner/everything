#pragma once

#include "sop/Group.h"

#include <map>
#include <functional>

namespace sop
{

class GroupMgr
{
public:
    GroupMgr() {}
    GroupMgr(const GroupMgr& groups);
    GroupMgr& operator = (const GroupMgr& groups);

    void Combine(const GroupMgr& groups,
        size_t pts_off, size_t vts_off, size_t prim_off);

    void Add(const std::shared_ptr<Group>& group, GroupMerge merge_op);
    std::shared_ptr<Group> Query(const std::string& name, size_t max_count = 0,
        GroupType type = GroupType::GuessFromGroup) const;
    void Rename(const std::string& src, const std::string& dst);

    void Traverse(std::function<bool(const Group&)> func) const;

private:
    std::map<std::string, std::shared_ptr<Group>> m_groups;

    size_t m_next_id = 0;

}; // GroupMgr

}