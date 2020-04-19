#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Merge : public Node
{
public:
    enum InPortIdx
    {
        IDX_SRC_A = 0,
        IDX_SRC_B,
    };

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

    virtual void Execute(const ur2::Device& dev, Evaluator& eval) override;

    virtual void AddInputPorts(size_t num) override;

    RTTR_ENABLE(Node)

}; // Merge

}
}