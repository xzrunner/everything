#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"

namespace sop
{
namespace node
{

class AttributeCreate : public Node
{
public:
    AttributeCreate()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void AddAttr(const std::string& name, GeoAttrClass cls, GeoAttrType type, VarValue val);
    void AddAttr(GeoAttr attr, GeoAttrClass cls, VarValue val);

private:
    struct Attr
    {
        std::string    name;
        GeoAttrClass    cls;
        GeoAttrType type;
        VarValue       value;
    };

private:
    std::vector<Attr> m_attrs;

    RTTR_ENABLE(Node)

}; // AttributeCreate

}
}