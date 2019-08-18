#pragma once

#include "everything/Node.h"
#include "everything/Group.h"

namespace evt
{
namespace node
{

class Blast : public Node
{
public:
    Blast()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    void SetGroupName(const std::string& name) { m_group_name = name; }

    void SetGroupType(GroupType type);

    void SetDeleteNonSelected(bool del_non_selected);

private:
    std::string m_group_name;

    GroupType m_group_type = GroupType::Points;

    bool m_delete_non_selected = false;

    RTTR_ENABLE(Node)

}; // Blast

}
}