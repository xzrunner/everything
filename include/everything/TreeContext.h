#pragma once

#include <map>
#include <memory>

namespace evt
{

struct BrushGroup;

class TreeContext
{
public:
    void AddGroup(const std::shared_ptr<BrushGroup>& group);
    std::shared_ptr<BrushGroup> QueryGroup(const std::string& name) const;

    void Combine(const TreeContext& ctx);

private:
    std::map<std::string, std::shared_ptr<BrushGroup>> m_groups;

}; // TreeContext

}