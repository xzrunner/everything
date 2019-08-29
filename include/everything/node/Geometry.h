#pragma once

#include "everything/Node.h"

#include <set>

namespace evt
{
namespace node
{

class Geometry : public Node
{
public:
    Geometry()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    static void AddChild(const std::shared_ptr<Geometry>& parent,
        const std::shared_ptr<Node>& child);

private:
    std::set<std::shared_ptr<Node>> m_children;

    RTTR_ENABLE(Node)

}; // Geometry

}
}