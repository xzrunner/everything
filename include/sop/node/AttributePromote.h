#pragma once

#include "sop/Node.h"
#include "sop/GeoAttrClass.h"

namespace sop
{
namespace node
{

class AttributePromote : public Node
{
public:
    AttributePromote()
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

#define PARM_FILEPATH "sop/node/AttributePromote.parm.h"
#include "sop/node_parms_gen.h"
#undef PARM_FILEPATH

}; // AttributePromote

}
}