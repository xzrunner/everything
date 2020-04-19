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

    virtual void Execute(const ur2::Device& dev, Evaluator& eval) override;

private:
    void Consolidate(const ur2::Device& dev);
    void ConsolidateBrush(const ur2::Device& dev);
    void ConsolidateShape();

    void UniquePoints(const ur2::Device& dev);
    void UniquePointsBrush(const ur2::Device& dev);
    void UniquePointsShape();

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Fuse.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Fuse

}
}