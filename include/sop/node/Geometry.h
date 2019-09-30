#pragma once

#include "sop/Node.h"

#include <set>

namespace sop
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

    virtual void Execute(Evaluator& eval) override;

    void ClearChildren() { m_children.clear(); }
    static void AddChild(const std::shared_ptr<Geometry>& parent,
        const std::shared_ptr<Node>& child);

    std::shared_ptr<Node> QueryChild(const std::string& name) const;

private:
    std::set<std::shared_ptr<Node>> m_children;

    RTTR_ENABLE(Node)

}; // Geometry

}
}