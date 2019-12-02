#pragma once

#include "sop/Node.h"
#include "sop/Group.h"

#include <SM_Vector.h>
#include <SM_Matrix.h>

namespace sop
{
namespace node
{

class Transform : public Node
{
public:
    Transform()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    static sm::mat4 CalcTransformMat(const sm::vec3& translate,
        const sm::vec3& rotate, const sm::vec3& scale, const sm::vec3& shear);

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Transform.parm.h"
#include <hdiop/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Transform

}
}