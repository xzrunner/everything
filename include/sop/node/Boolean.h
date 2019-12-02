#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Boolean : public Node
{
public:
    enum InPortIdx
    {
        IDX_A = 0,
        IDX_B,
    };

    enum class Operator
    {
        Union,
        Intersect,
        Subtract,
    };

    enum class GeoType
    {
        Solid,
        Surface,
    };

    enum class SubType
    {
        AMinusB,
        BMinusA,
        Both,
    };

public:
    Boolean()
    {
        m_imports = {
            {{ NodeVarType::Any, "a" }},
            {{ NodeVarType::Any, "b" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Boolean.parm.h"
#include <hdiop/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Boolean

}
}