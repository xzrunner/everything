#pragma once

#include "everything/Node.h"

#include <polymesh3/Geometry.h>

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
            {{ VariableType::Any, "in" }}
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    void SetGroupName(const std::string& name) { m_group_name = name; }

    void SetGroupType(pm3::GroupType type);

    void SetDeleteNonSelected(bool del_non_selected);

private:
    std::string m_group_name;

    pm3::GroupType m_group_type = pm3::GroupType::Points;

    bool m_delete_non_selected = false;

    RTTR_ENABLE(Node)

}; // Blast

}
}