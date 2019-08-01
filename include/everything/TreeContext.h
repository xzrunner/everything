#pragma once

#include <vector>
#include <memory>

namespace evt
{

struct BrushGroup;

class TreeContext
{
public:
    void AddGroup(const std::shared_ptr<BrushGroup>& group);
    std::shared_ptr<BrushGroup> QueryGroup(const std::string& name) const;

private:
    std::vector<std::shared_ptr<BrushGroup>> m_groups;

}; // TreeContext

}