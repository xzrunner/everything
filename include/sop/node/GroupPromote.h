#pragma once

#include "sop/Node.h"
#include "sop/Group.h"

namespace sop
{
namespace node
{

class GroupPromote : public Node
{
public:
    GroupPromote()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(const ur::Device& dev, Evaluator& eval) override;

private:
    void PrimsToPoints(Group& group);
    void PointsToPrims(Group& group);

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/GroupPromote.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // GroupPromote

}
}