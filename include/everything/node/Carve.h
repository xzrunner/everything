#pragma once

#include "everything/Node.h"

namespace evt
{
namespace node
{

class Carve : public Node
{
public:
    Carve()
    {
        m_imports = {
            {{ VariableType::Any, "in" }}
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    void SetFirstU(float u);
    void SetSecondU(float u);
    void SetFirstV(float v);
    void SetSecondV(float v);

private:
    float m_first_u  = 0;
    float m_second_u = 1;
    float m_first_v  = 0;
    float m_second_v = 1;

    RTTR_ENABLE(Node)

}; // Carve

}
}