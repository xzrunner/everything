#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class UVUnwrap : public Node
{
public:
    enum class ScaleType
    {
        None,
        Uniform,
        Stretch,
    };

public:
    UVUnwrap()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(const ur::Device& dev, Evaluator& eval) override;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/UVUnwrap.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // UVUnwrap

}
}