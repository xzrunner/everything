#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"

namespace sop
{
namespace node
{

class AttributeTransfer : public Node
{
public:
    enum InPortIdx
    {
        IDX_TO_GEO = 0,
        IDX_FROM_GEO,
    };

public:
    AttributeTransfer()
    {
        m_imports = {
            {{ NodeVarType::Any, "to" }},
            {{ NodeVarType::Any, "from" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/AttributeTransfer.parm.h"
#include "sop/node_parms_gen.h"
#undef PARM_FILEPATH

}; // AttributeTransfer

}
}