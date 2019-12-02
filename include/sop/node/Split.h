#pragma once

#include "sop/node/Subnetwork.h"

namespace sop
{
namespace node
{

class Blast;
class Output;

class Split : public Subnetwork
{
public:
    enum OutPortIdx
    {
        IDX_OUT_0 = 0,
        IDX_OUT_1,
    };

public:
    Split()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out0" }},
            {{ NodeVarType::Any, "out1" }},
        };

        InitSubNodes();
    }

    virtual void Execute(Evaluator& eval) override;

private:
    void InitSubNodes();

    RTTR_ENABLE(Subnetwork)

#define PARM_FILEPATH "sop/node/Split.parm.h"
#include <hdiop/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Split

}
}