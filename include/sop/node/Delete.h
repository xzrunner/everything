#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Delete : public Node
{
public:
    enum class NegateSelected
    {
        Delete,
        Keep,
    };

    enum class EntityType
    {
        Points,
        Edges,
        Faces,
    };

    enum class OpRule
    {
        Pattern,
        Range,
        Expression,
    };

public:
    Delete()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(const ur::Device& dev, Evaluator& eval) override;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Delete.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Delete

}
}