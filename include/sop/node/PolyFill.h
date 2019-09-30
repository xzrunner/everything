#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class PolyFill : public Node
{
public:
    PolyFill()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    RTTR_ENABLE(Node)

}; // PolyFill

}
}