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
    struct Item
    {
        std::string  name;
        GeoAttrClass cls  = GeoAttrClass::Point;
        GeoAttrType  type = GeoAttrType::Float;
        VarValue     value;
    };

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

    void SetAttrItems(const std::vector<Item>& items);

private:
    std::vector<Item> m_items;

    RTTR_ENABLE(Node)

}; // AttributeCreate

}
}