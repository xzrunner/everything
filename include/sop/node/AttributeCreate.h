#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"
#include "sop/Group.h"

namespace sop
{
namespace node
{

class AttributeCreate : public Node
{
public:
    enum class ItemType
    {
        Float,
        Integer,
        Vector,
        String,
        FloatArray,
        IntegerArray,
        StringArray,
    };

    enum class ItemFltInfo
    {
        Guess,
        None,
        Position,
        Vector,
        Normal,
        Color,
        Quaternion,
        TransformMatrix,
        TextureCoordinate,
    };

    struct Item
    {
        Item() {}
        Item(const std::string& name, ItemType type, GeoAttrClass cls, const VarValue& val, const VarValue& default_val);

        bool operator == (const Item& i) const;

        std::string  name;
        GeoAttrClass cls  = GeoAttrClass::Point;
        ItemType     type = ItemType::Float;
        VarValue     value;
        VarValue     default_val;
        std::string  string;
        ItemFltInfo  flt_info = ItemFltInfo::Guess;
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