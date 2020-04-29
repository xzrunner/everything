#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class UVLayout : public Node
{
public:
    UVLayout()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(const ur::Device& dev, Evaluator& eval) override;

    RTTR_ENABLE(Node)

}; // UVLayout

}
}