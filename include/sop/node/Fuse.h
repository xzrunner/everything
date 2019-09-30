#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Fuse : public Node
{
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

    void SetDistance(float dist);

private:
    void FuseBrush();
    void FuseShape();

private:
    float m_distance = 0.001f;

    RTTR_ENABLE(Node)

}; // Fuse

}
}