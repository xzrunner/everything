#pragma once

#include "sop/Node.h"
#include "sop/typedef.h"

namespace sop
{
namespace node
{

class Lattice : public Node
{
public:
    enum InPortIdx
    {
        IDX_DATA_SOURCE = 0,
        IDX_INITIAL_SOURCE,
        IDX_DEFORMED_SOURCE,
    };

public:
    Lattice()
    {
        m_imports = {
            {{ NodeVarType::Any, "data" }},
            {{ NodeVarType::Any, "initial" }},
            {{ NodeVarType::Any, "deformed" }},
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    RTTR_ENABLE(Node)

}; // Lattice

}
}