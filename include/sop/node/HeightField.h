#pragma once

#include "sop/Node.h"
#include "sop/GeoAttrClass.h"
#include "sop/Widgets.h"

#include <SM_Vector.h>

namespace sop
{
namespace node
{

class HeightField : public Node
{
public:
    HeightField()
    {
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(const ur2::Device& dev, Evaluator& eval) override;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/HeightField.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // HeightField

}
}