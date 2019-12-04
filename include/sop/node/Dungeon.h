#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Dungeon : public Node
{
public:
    Dungeon()
    {
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

private:
    void BuildModel();

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Dungeon.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Dungeon

}
}