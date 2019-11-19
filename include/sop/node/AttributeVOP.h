#pragma once

#include "sop/Node.h"

namespace vop
{
class Evaluator;
namespace node
{
    class GeoGlobalParams;
    class GeoOutputVars;
}
}

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

private:
    void InitEval();

private:
    std::shared_ptr<vop::Evaluator> m_eval = nullptr;

    std::shared_ptr<vop::node::GeoGlobalParams> m_input = nullptr;
    std::shared_ptr<vop::node::GeoOutputVars> m_output = nullptr;

    RTTR_ENABLE(Node)

}; // AttributeVOP

}
}