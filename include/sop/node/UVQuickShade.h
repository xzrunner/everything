#pragma once

#include "sop/Node.h"

#include <SM_Vector.h>

namespace sop
{
namespace node
{

class UVQuickShade : public Node
{
public:
    UVQuickShade()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

private:
    int m_diffuse_tex = -1;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/UVQuickShade.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // UVQuickShade

}
}