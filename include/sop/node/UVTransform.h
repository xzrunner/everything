#pragma once

#include "sop/Node.h"
#include "sop/Group.h"

#include <SM_Vector.h>
#include <SM_Matrix.h>

namespace sop
{
namespace node
{

class UVTransform : public Node
{
public:
    UVTransform()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetGroupName(const std::string& name);
    void SetGroupType(GroupType type);

private:
    std::string m_group_name;
    GroupType   m_group_type = GroupType::GuessFromGroup;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/UVTransform.parm.h"
#include "sop/node_parms_gen.h"
#undef PARM_FILEPATH

}; // UVTransform

}
}