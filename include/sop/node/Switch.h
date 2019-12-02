#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Switch : public Node
{
public:
    Switch()
    {
        m_imports = {
            {{ NodeVarType::Any, "in0" }},
            {{ NodeVarType::Any, "in1" }},
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    virtual void AddInputPorts(size_t num) override;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Switch.parm.h"
#include <hdiop/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Switch

}
}