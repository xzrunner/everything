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
            {{ VariableType::Any, "in0" }},
            {{ VariableType::Any, "in1" }},
            {{ VariableType::Any, "in2" }},
            {{ VariableType::Any, "in3" }},
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    RTTR_ENABLE(Node)

}; // Merge

}
}