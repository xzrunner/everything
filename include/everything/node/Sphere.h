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
            {{ VariableType::Port, "prev" }}
        };
        m_exports = {
            {{ VariableType::Port, "next" }},
        };
    }

    virtual void Execute() override;
    virtual void Traverse(std::function<bool(const n0::SceneNodePtr&)> func) const override;

private:
    gs::Sphere m_sphere;

    n0::SceneNodePtr m_node = nullptr;

    RTTR_ENABLE(Node)

}; // Sphere

}
}