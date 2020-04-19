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

    virtual void Execute(const ur2::Device& dev, Evaluator& eval) override;

private:
    std::string FormatExprStr() const;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/AttributeWrangle.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // AttributeWrangle

}
}