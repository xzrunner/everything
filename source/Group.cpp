#include "sop/Group.h"

#include <set>

#include <assert.h>

namespace sop
{

bool Group::Add(size_t item)
{
    for (auto& i : m_items) {
        if (i == item) {
            return false;
        }
    }

    m_items.push_back(item);

    return true;
}

void Group::Clear()
{
    m_items.clear();
}

void Group::Offset(size_t off)
{
    for (auto& i : m_items) {
        i += off;
    }
}

void Group::Merge(GroupMerge op, const std::vector<size_t>& src, std::vector<size_t>& dst)
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