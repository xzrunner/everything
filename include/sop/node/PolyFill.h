#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class PolyFill : public Node
{
public:
    enum class Mode
    {
        SinglePolygon,
        Triangles,
        TriangleFan,
        QuadrilateralFan,
        Quadrilaterals,
        QuadrilateralGrid,
    };

public:
    PolyFill()
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

#define PARM_FILEPATH "sop/node/PolyFill.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // PolyFill

}
}