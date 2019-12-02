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

private:
    NodePtr FindForeachBegin() const;
    std::set<NodePtr> FindClosureNodes(const NodePtr& begin) const;

    void DoOnePass(Evaluator& sub_eval, const NodePtr& begin,
        const std::shared_ptr<GeometryImpl>& prev_geo, size_t idx);

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/ForeachPrimEnd.parm.h"
#include <hdiop/node_parms_gen.h>
#undef PARM_FILEPATH

}; // ForeachPrimEnd

}
}