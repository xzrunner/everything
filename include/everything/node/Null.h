#pragma once

#include "everything/Node.h"

namespace evt
{
namespace node
{

class Null : public Node
{
public:
    Null()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override {}

    RTTR_ENABLE(Node)

}; // Null

}
}