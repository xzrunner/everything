#pragma once

#include "sop/Node.h"
#include "sop/typedef.h"

namespace sop
{
namespace node
{

class ForeachPrimEnd : public Node
{
public:
    enum class IterMethod
    {
        AutoDetectFromInputs,
        ByPiecesOrPoints,
        ByCount,
    };

    enum class GatherMethod
    {
        Feedback,
        Merge,
    };

    enum class PieceElements
    {
        Primitives,
        Points,
    };

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