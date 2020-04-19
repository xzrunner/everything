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

    virtual void Execute(const ur2::Device& dev, Evaluator& eval) override;

private:
    void BuildModel(const ur2::Device& dev);

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Grid.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Grid

}
}