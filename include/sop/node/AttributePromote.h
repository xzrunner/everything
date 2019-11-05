#pragma once

#include "sop/Node.h"
#include "sop/GeoAttrClass.h"

namespace sop
{
namespace node
{

class AttributePromote : public Node
{
public:
    AttributePromote()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetAttrName(const std::string& name);
    void SetPromoteType(GeoAttrClass from, GeoAttrClass to);

private:
    std::string m_attr_name;

    GeoAttrClass m_from_type = GeoAttrClass::Point;
    GeoAttrClass m_to_type   = GeoAttrClass::Point;

    RTTR_ENABLE(Node)

}; // AttributePromote

}
}