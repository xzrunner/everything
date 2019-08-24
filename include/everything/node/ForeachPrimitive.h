#pragma once

#include "everything/Node.h"
#include "everything/Evaluator.h"

namespace evt
{
namespace node
{

class ForeachPrimitive : public Node
{
public:
    ForeachPrimitive()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    void SetNodes(const std::vector<std::shared_ptr<Node>>& nodes,
        size_t idx_begin, size_t idx_end);

private:
    class SubNode : public Node
    {
    public:
        SubNode();
        virtual void Execute(TreeContext& ctx) override {}
    }; // SubNode

private:
    Evaluator m_eval;

    std::shared_ptr<Node> m_begin = nullptr;
    std::shared_ptr<Node> m_end   = nullptr;

    RTTR_ENABLE(Node)

}; // ForeachPrimitive

}
}