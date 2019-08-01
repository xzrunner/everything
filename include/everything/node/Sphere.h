#pragma once

#include "everything/Node.h"

#include <geoshape/Sphere.h>
#include <node0/typedef.h>

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

protected:
    virtual void ExecuteSelf() override;

private:
    gs::Sphere m_sphere;

    RTTR_ENABLE(Node)

}; // Sphere

}
}