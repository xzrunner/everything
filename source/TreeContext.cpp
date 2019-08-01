#include "everything/TreeContext.h"
#include "everything/BrushGroup.h"

namespace evt
{

void TreeContext::AddGroup(const std::shared_ptr<BrushGroup>& group)
{
    m_groups.push_back(group);
}

std::shared_ptr<BrushGroup>
TreeContext::QueryGroup(const std::string& name) const
{
    if (name.empty()) {
        return nullptr;
    }

    for (auto& g : m_groups) {
        if (g->name == name) {
            return g;
        }
    }

    return nullptr;
}

}