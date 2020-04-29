#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class ForeachPrimBegin : public Node
{
public:
    enum class Method
    {
        Feedback,
        PieceOrPoint,
        Metadata,
        Input,
    };

public:
    ForeachPrimBegin()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(const ur::Device& dev, Evaluator& eval) override {}

    void SetGeoImpl(const std::shared_ptr<GeometryImpl>& geo_impl) {
        m_geo_impl = geo_impl;
    }

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/ForeachPrimBegin.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // ForeachPrimBegin

}
}