#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Python : public Node
{
public:
    Python()
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

#define PARM_FILEPATH "sop/node/Python.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Python

}
}