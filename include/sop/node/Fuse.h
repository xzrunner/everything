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

    void SetFuseOP(Operator op);

    // Consolidate
    void SetDistance(float dist);

private:
    void Consolidate();
    void ConsolidateBrush();
    void ConsolidateShape();

    void UniquePoints();
    void UniquePointsBrush();
    void UniquePointsShape();

private:
    Operator m_fuse_op = Operator::Consolidate;

    // Consolidate
    float m_distance = 0.001f;

    RTTR_ENABLE(Node)

}; // Fuse

}
}