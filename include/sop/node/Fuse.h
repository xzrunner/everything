#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Fuse : public Node
{
public:
    enum class Operator
    {
        Consolidate,
        UniquePoints,
        Snap,
    };

public:
    Fuse()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }},
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

private:
    void Consolidate();
    void ConsolidateBrush();
    void ConsolidateShape();

    void UniquePoints();
    void UniquePointsBrush();
    void UniquePointsShape();

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Fuse.parm.h"
#include <hdiop/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Fuse

}
}