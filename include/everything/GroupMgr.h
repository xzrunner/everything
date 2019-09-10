#pragma once

#include "everything/Group.h"

#include <map>

namespace evt
{

class GroupMgr
{
public:
    GroupMgr() {}
    GroupMgr(const GroupMgr& groups);
    GroupMgr& operator = (const GroupMgr& groups);

    void Combine(const GroupMgr& groups, size_t prim_off);

    void Add(const std::shared_ptr<Group>& group);
    std::shared_ptr<Group> Query(const std::string& name) const;
    void Rename(const std::string& src, const std::string& dst);

    static void Merge(GroupMerge op, const std::vector<size_t>& src,
        std::vector<size_t>& dst);

private:
    std::map<std::string, std::shared_ptr<Group>> m_groups;

    size_t m_next_id = 0;

}; // GroupMgr

}