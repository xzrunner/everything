#pragma once

#include "everything/Node.h"

#include <geoshape/Box.h>
#include <node0/typedef.h>

namespace n3 { class CompTransform; }

namespace evt
{
namespace node
{

class Box : public Node
{
public:
    Box()
    {
        m_imports = {
            {{ VariableType::Port, "prev" }}
        };
        m_exports = {
            {{ VariableType::Port,      "next" }},
//            {{ VariableType::Primitive, "prim" }}
        };
    }

    virtual void Execute() override;
    virtual void Traverse(std::function<bool(const n0::SceneNodePtr&)> func) const override;

private:
    gs::Box m_box;

    n0::SceneNodePtr m_node = nullptr;

    RTTR_ENABLE(Node)

}; // Box

}
}