#pragma once

#include "everything/Node.h"
#include "everything/typedef.h"

namespace evt
{
namespace node
{

class ForeachPrimEnd : public Node
{
public:
    ForeachPrimEnd()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval, TreeContext& ctx) override;

private:
    NodePtr FindForeachBegin(std::vector<NodePtr>& nodes) const;

    RTTR_ENABLE(Node)

}; // ForeachPrimEnd

}
}