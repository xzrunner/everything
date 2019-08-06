#include "everything/TreeContext.h"
#include "everything/BrushGroup.h"

namespace evt
{

void TreeContext::AddGroup(const std::shared_ptr<BrushGroup>& group)
{
    if (group->name.empty()) {
        return;
    }

    m_groups.insert({ group->name, group });
}

std::shared_ptr<BrushGroup>
TreeContext::QueryGroup(const std::string& name) const
{
    if (name.empty()) {
        return nullptr;
    }

    auto itr = m_groups.find(name);
    return itr == m_groups.end() ? nullptr : itr->second;
}

void TreeContext::Combine(const TreeContext& ctx)
{
    for (auto& pair : ctx.m_groups) {
        m_groups.insert({ pair.first, pair.second });
    }
}

}