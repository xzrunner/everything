#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"

#include <SM_Vector.h>
#include <SM_Matrix.h>

namespace sop
{
namespace node
{

class Primitive : public Node
{
public:
    Primitive()
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
    void UpdatePrim(GeoAttribute::Primitive& prim, const sm::mat4& mat);

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Primitive.parm.h"
#include <hdiop/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Primitive

}
}