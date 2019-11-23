#pragma once

#include "sop/Node.h"
#include "sop/Group.h"

namespace sop
{
namespace node
{

class GroupExpression : public Node
{
public:
    GroupExpression()
    {
        m_imports = {
            {{ NodeVarType::Any, "in0" }},
            {{ NodeVarType::Any, "in1" }},
            {{ NodeVarType::Any, "in2" }},
            {{ NodeVarType::Any, "in3" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetExprsNum(size_t num);
    size_t GetExprsNum() const;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/GroupExpression.parm.h"
#include "sop/node_parms_gen.h"
#undef PARM_FILEPATH

}; // GroupExpression

}
}