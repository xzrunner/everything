#include "sop/GroupMgr.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

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
GroupMgr::Query(const std::string& name, size_t max_count, GroupType type) const
{
    auto itr = m_groups.find(name);
    if (itr != m_groups.end()) 
    {
        auto group = itr->second;
        if (type != GroupType::GuessFromGroup && type != group->GetType()) {
            return nullptr;
        } else {
            return group;
        }
    }

    std::vector<std::string> tokens;
    boost::split(tokens, name, boost::is_any_of(" "));

    std::vector<size_t> indices;
    indices.reserve(tokens.size());
    for (auto& str : tokens) 
    {
        int i = -1;
        try {
            i = boost::lexical_cast<int>(str);
        } catch (boost::bad_lexical_cast&) {
            continue;
        }
        if (i >= 0 && i < static_cast<int>(max_count)) {
            indices.push_back(i);
        }
    }

    if (indices.empty()) {
        return nullptr;
    }

    std::sort(indices.begin(), indices.end());
    indices.erase(std::unique(indices.begin(), indices.end()), indices.end());

    auto group = std::make_shared<Group>();
    group->SetType(type);
    group->SetItems(indices);
    return group;
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