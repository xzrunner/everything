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
        Item(const std::string& name, GeoAttrType type, GeoAttrClass cls, const VarValue& val);
        Item(sop::GeoAttr attr, GeoAttrClass cls, const VarValue& val);

        bool operator == (const Item& i) const;

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

    void SetGroupName(const std::string& name);
    void SetGroupType(GroupType type);

    void SetAttrItems(const std::vector<Item>& items);

private:
    std::string m_group_name;
    GroupType   m_group_type = GroupType::GuessFromGroup;

    std::vector<Item> m_items;

    RTTR_ENABLE(Node)

}; // AttributeCreate

}
}