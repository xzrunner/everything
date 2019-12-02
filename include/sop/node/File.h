#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class File : public Node
{
public:
    File()
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

#define PARM_FILEPATH "sop/node/File.parm.h"
#include <hdiop/node_parms_gen.h>
#undef PARM_FILEPATH

}; // File

}
}