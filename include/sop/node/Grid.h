#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Grid : public Node
{
public:
    Grid()
    {
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

private:
    void BuildModel();

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Grid.parm.h"
#include <hdiop/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Grid

}
}