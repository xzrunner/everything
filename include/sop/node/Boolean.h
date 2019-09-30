#pragma once

#include "sop/Node.h"

namespace sop
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

    enum class TreatAs
    {
        Solid,
        Surface,
    };

public:
    Boolean()
    {
        m_imports = {
            {{ NodeVarType::Any, "a" }},
            {{ NodeVarType::Any, "b" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetOperator(Operator op);

    void SetTreatAs(TreatAs a, TreatAs b);

private:
    Operator m_operator = Operator::Intersect;

    TreatAs m_a_type = TreatAs::Solid;
    TreatAs m_b_type = TreatAs::Solid;

    RTTR_ENABLE(Node)

}; // Boolean

}
}