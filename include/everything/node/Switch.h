#pragma once

#include "everything/Node.h"

namespace evt
{
namespace node
{

class Switch : public Node
{
public:
    Switch()
    {
        m_imports = {
            {{ NodeVarType::Any, "in0" }},
            {{ NodeVarType::Any, "in1" }},
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    virtual void AddInputPorts(size_t num) override;

    void SetSelected(size_t select);

private:
    size_t m_selected = 0;

    RTTR_ENABLE(Node)

}; // Switch

}
}