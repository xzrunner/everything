#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Output : public Node
{
public:
    Output()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetOutputIdx(int idx) { m_output_idx = idx; }
    int  GetOutputIdx() const { return m_output_idx; }

private:
    int m_output_idx = -1;

    RTTR_ENABLE(Node)

}; // Output

}
}