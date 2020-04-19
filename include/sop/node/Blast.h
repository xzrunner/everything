#pragma once

#include "sop/Node.h"
#include "sop/Group.h"
#include "sop/GeoAttrClass.h"

namespace sop
{
namespace node
{

class Blast : public Node
{
public:
    Blast()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(const ur2::Device& dev, Evaluator& eval) override;

private:
    bool SetupDelFlags(const Group& group, size_t count,
        std::vector<bool>& del_flags) const;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Blast.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Blast

}
}