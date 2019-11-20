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

    enum class GeoType
    {
        Solid,
        Surface,
    };

    enum class SubType
    {
        AMinusB,
        BMinusA,
        Both,
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

    void SetGeoType(GeoType a, GeoType b);

private:
    Operator m_operator = Operator::Intersect;

    GeoType m_a_type = GeoType::Solid;
    GeoType m_b_type = GeoType::Solid;

    RTTR_ENABLE(Node)

}; // Boolean

}
}