#pragma once

#include "everything/Node.h"
#include "everything/GeoAttribute.h"

namespace evt
{
namespace node
{

class AttributeWrangle : public Node
{
public:
    AttributeWrangle()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetVexExpr(const std::string& expr);

private:
    std::string m_vex_expr;

    RTTR_ENABLE(Node)

}; // AttributeWrangle

}
}