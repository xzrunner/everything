#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Divide : public Node
{
public:
    enum InPortIdx
    {
        IDX_INPUT = 0,
        IDX_REST,
    };

public:
    Divide()
    {
        m_imports = {
            {{ NodeVarType::Any, "input" }},
            {{ NodeVarType::Any, "rest" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    RTTR_ENABLE(Node)

}; // Divide

}
}