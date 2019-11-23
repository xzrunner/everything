#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"

namespace sop
{
namespace node
{

class AttributeWrangle : public Node
{
public:
    AttributeWrangle()
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

#define PARM_FILEPATH "sop/node/AttributeWrangle.parm.h"
#include "sop/node_parms_gen.h"
#undef PARM_FILEPATH

}; // AttributeWrangle

}
}