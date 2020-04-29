#pragma once

#include "sop/Node.h"

#include <SM_Vector.h>

namespace model { class BrushModel; }

namespace sop
{
namespace node
{

class Box : public Node
{
public:
    Box()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(const ur::Device& dev, Evaluator& eval) override;

private:
    void BuildModel(const ur::Device& dev);

    std::unique_ptr<model::BrushModel> BuildBrush() const;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Box.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Box

}
}