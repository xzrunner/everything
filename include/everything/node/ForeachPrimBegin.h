#pragma once

#include "everything/Node.h"

namespace evt
{
namespace node
{

class ForeachPrimBegin : public Node
{
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

    virtual void Execute(Evaluator& eval, TreeContext& ctx) override {}

    void SetGeoImpl(const std::shared_ptr<GeometryImpl>& geo_impl) {
        m_geo_impl = geo_impl;
    }

    RTTR_ENABLE(Node)

}; // ForeachPrimBegin

}
}