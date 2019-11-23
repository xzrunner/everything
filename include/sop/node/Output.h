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

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Output.parm.h"
#include "sop/node_parms_gen.h"
#undef PARM_FILEPATH

}; // Output

}
}