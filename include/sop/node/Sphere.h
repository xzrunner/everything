#pragma once

#include "sop/Node.h"

#include <SM_Vector.h>

namespace sop
{
namespace node
{

class Sphere : public Node
{
public:
    Sphere()
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
    void BuildModel();

private:
    sm::vec3 m_center = sm::vec3(0, 0, 0);
    float    m_radius = 1.0f;

    RTTR_ENABLE(Node)

}; // Sphere

}
}