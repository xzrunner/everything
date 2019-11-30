#pragma once

#include "sop/Node.h"

namespace vop { class Evaluator; class Node; }

namespace sop
{
namespace node
{

class AttributeVOP : public Node
{
public:
    AttributeVOP()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };

        InitEval();
    }

    virtual void Execute(Evaluator& eval) override;

    void SetEval(const std::shared_ptr<vop::Evaluator>& eval);
    auto GetEval() const { return m_eval; }

private:
    void InitEval();

private:
    std::shared_ptr<vop::Evaluator> m_eval = nullptr;

    RTTR_ENABLE(Node)

}; // AttributeVOP

}
}