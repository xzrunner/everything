#pragma once

#include "sop/Node.h"
#include "sop/Group.h"

namespace sop
{
namespace node
{

class Peak : public Node
{
public:
    Peak()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

private:
    void TranslatePoints(float dist, const std::shared_ptr<Group>& group);
    void TranslateVertices(float dist, const std::shared_ptr<Group>& group);
    void TranslatePrimitives(float dist, const std::shared_ptr<Group>& group);

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Peak.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Peak

}
}