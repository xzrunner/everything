#pragma once

#include "sop/Node.h"

#include <SM_Vector.h>

namespace sop
{
namespace node
{

class Carve : public Node
{
public:
    enum PropID
    {
        FIRST_U,
        SECOND_U,
        FIRST_V,
        SECOND_V,

        MAX_BUILD_IN_PROP,
    };

    static const constexpr char* const PropNames[MAX_BUILD_IN_PROP] = {
        "domainu1", "domainu2", "domainv1", "domainv2"
    };

public:
    Carve()
        : Node(MAX_BUILD_IN_PROP)
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };

        InitProps();
    }

    virtual void Execute(Evaluator& eval) override;

    void SetFirstU(float u);
    void SetSecondU(float u);
    void SetFirstV(float v);
    void SetSecondV(float v);

private:
    void InitProps();

    RTTR_ENABLE(Node)

}; // Carve

}
}