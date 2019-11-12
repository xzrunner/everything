#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Python : public Node
{
public:
    Python()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetCode(const std::string& code);

private:
    std::string m_code;

    RTTR_ENABLE(Node)

}; // Python

}
}