#include "sop/node/AttributeCreate.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

namespace sop
{
namespace node
{

void AttributeCreate::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    std::shared_ptr<Group> group = nullptr;
    if (!m_group_name.empty())
    {
        group = m_geo_impl->GetGroup().Query(m_group_name);
        if (!group) {
            return;
        }
    }

    for (auto& item : m_items)
    {
        size_t num = 0;
        switch (item.cls)
        {
        case GeoAttrClass::Point:
            num = m_geo_impl->GetAttr().GetPoints().size();
            break;
        case GeoAttrClass::Vertex:
            num = m_geo_impl->GetAttr().GetVertices().size();
            break;
        case GeoAttrClass::Primitive:
            num = m_geo_impl->GetAttr().GetPrimtives().size();
            break;
        case GeoAttrClass::Detail:
            num = 1;
            break;
        default:
            assert(0);
        }

        std::vector<VarValue> vars;
        if (group)
        {
            vars.resize(num, item.default_val);
            for (auto& i : group->GetItems()) {
                vars[i] = item.value;
            }
        }
        else
        {
            vars.resize(num, item.value);
        }

        GeoAttrType type;
        switch (item.type)
        {
        case sop::node::AttributeCreate::ItemType::Float:
            type = GeoAttrType::Float;
            break;
        case sop::node::AttributeCreate::ItemType::Integer:
            type = GeoAttrType::Int;
            break;
        case sop::node::AttributeCreate::ItemType::Vector:
            type = GeoAttrType::Vector;
            break;
        case sop::node::AttributeCreate::ItemType::String:
            type = GeoAttrType::String;
            break;
        default:
            assert(0);
        }
        m_geo_impl->GetAttr().AddAttr(item.cls, GeoAttribute::VarDesc(item.name, type), vars);
    }
}
    }
}

void AttributeCreate::SetGroupName(const std::string& name)
{
    NODE_PROP_SET(m_group_name, name);
}

void AttributeCreate::SetGroupType(GroupType type)
{
    NODE_PROP_SET(m_group_type, type);
}

void AttributeCreate::SetAttrItems(const std::vector<Item>& items)
{
    NODE_PROP_SET(m_items, items);
}

//////////////////////////////////////////////////////////////////////////
// struct AttributeCreate::Item
//////////////////////////////////////////////////////////////////////////

AttributeCreate::Item::
Item(const std::string& name, ItemType type, GeoAttrClass cls, const VarValue& val, const VarValue& default_val)
    : name(name)
    , cls(cls)
    , type(type)
    , value(val)
    , default_val(default_val)
{
}

bool AttributeCreate::Item::
operator == (const Item& i) const
{
    if (name != i.name ||
        cls != i.cls ||
        type != i.type) {
        return false;
    }

    switch (type)
    {
    case sop::node::AttributeCreate::ItemType::Float:
        if (value.f != i.value.f) {
            return false;
        }
        break;
    case sop::node::AttributeCreate::ItemType::Integer:
        if (value.i != i.value.i) {
            return false;
        }
        break;
    case sop::node::AttributeCreate::ItemType::Vector:
        if (*static_cast<const sm::vec3*>(value.p) !=
            *static_cast<const sm::vec3*>(i.value.p)) {
            return false;
        }
        break;
    case sop::node::AttributeCreate::ItemType::String:
        if (value.p == nullptr || i.value.p == nullptr ||
            strcmp(static_cast<const char*>(value.p), static_cast<const char*>(i.value.p)) != 0) {
            return false;
        }
        break;

    default:
        assert(0);
    }

    return true;
}

}
}