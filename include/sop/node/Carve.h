#pragma once

#include "sop/Node.h"

#include <SM_Vector.h>

namespace sop
{
namespace node
{

class Carve : public Node
{
public:
    Carve()
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

#define PARM_FILEPATH "sop/node/Carve.parm.h"
#include <hdiop/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Carve

}
}