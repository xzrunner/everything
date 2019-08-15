#pragma once

#include "everything/Node.h"

#include <SM_Vector.h>

namespace evt
{
namespace node
{

class Sphere : public Node
{
public:
    Sphere()
    {
        m_imports = {
            {{ VariableType::Any, "in" }}
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

private:
    void BuildModel();

private:
    sm::vec3 m_center = sm::vec3(0, 0, 0);
    float    m_radius = 1.0f;

    RTTR_ENABLE(Node)

}; // Sphere

}
}