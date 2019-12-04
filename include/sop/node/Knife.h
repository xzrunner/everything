#pragma once

#include "sop/Node.h"

#include <SM_Vector.h>

namespace pm3 { class Polytope; }

namespace sop
{
namespace node
{

class Knife : public Node
{
public:
    enum class KeepType
    {
        KeepAbove,
        KeepBelow,
        KeepAll,
    };

public:
    Knife()
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
    bool Clip(pm3::Polytope& poly) const;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Knife.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Knife

}
}