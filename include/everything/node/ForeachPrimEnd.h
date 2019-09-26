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

    virtual void Execute(Evaluator& eval) override;

    void EnableSinglePass(bool do_single_pass);

    void SetSinglePassOffset(int single_pass_offset);

private:
    NodePtr FindForeachBegin() const;
    std::set<NodePtr> FindClosureNodes(const NodePtr& begin) const;

    void DoOnePass(Evaluator& sub_eval, const NodePtr& begin,
        const std::shared_ptr<GeometryImpl>& prev_geo, size_t idx);

private:
    bool m_do_single_pass     = false;
    int  m_single_pass_offset = 0;

    RTTR_ENABLE(Node)

}; // ForeachPrimEnd

}
}