#pragma once

#include "everything/Node.h"

namespace evt
{
namespace node
{

class Boolean : public Node
{
public:
    enum InPortIdx
    {
        IDX_A = 0,
        IDX_B,
    };

    enum class Operator
    {
        Union,
        Intersect,
        Subtract,
    };

public:
    Boolean()
    {
        m_imports = {
            {{ VariableType::Any, "a" }},
            {{ VariableType::Any, "b" }}
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    void SetOperator(Operator op);

private:
    Operator m_operator = Operator::Intersect;

    RTTR_ENABLE(Node)

}; // Boolean

}
}