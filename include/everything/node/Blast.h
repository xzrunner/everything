#pragma once

#include "everything/Node.h"
#include "everything/node/typedef.h"

namespace evt
{

struct BrushGroup;

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

    virtual void BeforeUpdateContext() override;

    void SetGroupName(const std::string& name) { m_group_name = name; }

    void SetGroupType(GroupType type);

    void SetDeleteNonSelected(bool del_non_selected);

protected:
    virtual void ExecuteSelf() override;
    virtual void UpdateCtxSelf(TreeContext& ctx) override;

private:
    std::string m_group_name;

    std::shared_ptr<BrushGroup> m_group = nullptr;

    GroupType m_group_type = GroupType::Points;

    bool m_delete_non_selected = false;

    RTTR_ENABLE(Node)

}; // Blast

}
}