#pragma once

#include "sop/Node.h"
#include "sop/GeoAttrClass.h"
#include "sop/Widgets.h"

#include <SM_Vector.h>

namespace sop
{
namespace node
{

class Color : public Node
{
public:
    Color()
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

#define PARM_FILEPATH "sop/node/Color.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Color

}
}