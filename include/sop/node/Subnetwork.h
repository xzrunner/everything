#pragma once

#include "sop/Node.h"

#include <set>

namespace sop
{
namespace node
{

class Subnetwork : public Node
{
public:
    Subnetwork()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    auto& GetAllChildren() const { return m_children; }

    void ClearChildren() { m_children.clear(); }
    static void AddChild(const std::shared_ptr<Subnetwork>& parent,
        const std::shared_ptr<Node>& child);

    std::shared_ptr<Node> QueryChild(const std::string& name) const;

    std::shared_ptr<Node> QueryOutput(int idx) const;

protected:
    std::vector<std::shared_ptr<Node>> m_children;

    RTTR_ENABLE(Node)

}; // Subnetwork

}
}