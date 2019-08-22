#pragma once

#include "everything/Node.h"

namespace evt
{
namespace node
{

class Merge : public Node
{
public:
    Merge()
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

    RTTR_ENABLE(Node)

}; // Merge

}
}