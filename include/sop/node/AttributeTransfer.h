#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"

namespace sop
{
namespace node
{

class AttributeTransfer : public Node
{
public:
    enum InPortIdx
    {
        IDX_TO_GEO = 0,
        IDX_FROM_GEO,
    };

public:
    AttributeTransfer()
    {
        m_imports = {
            {{ NodeVarType::Any, "to" }},
            {{ NodeVarType::Any, "from" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetCopyAttrs(GeoAttrClass cls, const std::vector<std::string>& attrs);

private:
    std::vector<std::string> m_attrs[static_cast<int>(GeoAttrClass::MaxTypeNum)];

    RTTR_ENABLE(Node)

}; // AttributeTransfer

}
}