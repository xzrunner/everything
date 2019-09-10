#pragma once

#include "everything/Node.h"

namespace evt
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

    virtual void Execute(Evaluator& eval, TreeContext& ctx) override;

    RTTR_ENABLE(Node)

}; // PolyFill

}
}