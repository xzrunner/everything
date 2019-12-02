#pragma once

#include "sop/Node.h"

#include <SM_Vector.h>

namespace sop
{
namespace node
{

class Line : public Node
{
public:
    Line()
    {
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

private:
    void BuildModel();

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Line.parm.h"
#include <hdiop/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Line

}
}