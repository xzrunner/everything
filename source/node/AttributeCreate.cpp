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

    assert(IsNumberValid());
    for (size_t i = 0, n = m_item_names.size(); i < n; ++i)
    {
        size_t num = 0;
        switch (m_item_classes[i])
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
        // int
        if (m_item_types[i] == ItemType::Integer && m_item_comp_sizes[i] == 1)
        {
            if (group)
            {
                vars.resize(num, VarValue(static_cast<int>(m_item_default_vals[i].x)));
                for (auto& i : group->GetItems()) {
                    vars[i] = VarValue(static_cast<int>(m_item_values[i].x));
                }
            }
            else
            {
                vars.resize(num, VarValue(static_cast<int>(m_item_values[i].x)));
            }
        }
        // float
        else if (m_item_types[i] == ItemType::Float && m_item_comp_sizes[i] == 1)
        {
            if (group)
            {
                vars.resize(num, VarValue(static_cast<float>(m_item_default_vals[i].x)));
                for (auto& i : group->GetItems()) {
                    vars[i] = VarValue(static_cast<float>(m_item_values[i].x));
                }
            }
            else
            {
                vars.resize(num, VarValue(static_cast<float>(m_item_values[i].x)));
            }
        }
        // float3
        else if ((m_item_types[i] == ItemType::Float || m_item_types[i] == ItemType::Vector)
              && m_item_comp_sizes[i] == 3)
        {
            if (group)
            {
                vars.resize(num, VarValue(static_cast<sm::vec3>(m_item_default_vals[i].xyzw)));
                for (auto& i : group->GetItems()) {
                    vars[i] = VarValue(static_cast<sm::vec3>(m_item_values[i].xyzw));
                }
            }
            else
            {
                vars.resize(num, VarValue(static_cast<sm::vec3>(m_item_values[i].xyzw)));
            }
        }
        // string
        else if (m_item_types[i] == ItemType::String)
        {
            if (group)
            {
                vars.resize(num, VarValue(std::string()));
                for (auto& i : group->GetItems()) {
                    vars[i] = VarValue(m_item_strings[i]);
                }
            }
            else
            {
                vars.resize(num, VarValue(m_item_strings[i]));
            }
        }
        // todo
        else
        {
            assert(0);
        }

        GeoAttrType type;
        switch (m_item_types[i])
        {
        case sop::node::AttributeCreate::ItemType::Integer:
        {
            switch (m_item_comp_sizes[i])
            {
            case 1:
                type = GeoAttrType::Int;
                break;
            case 2:
                type = GeoAttrType::Int2;
                break;
            case 3:
                type = GeoAttrType::Int3;
                break;
            case 4:
                type = GeoAttrType::Int4;
                break;
            }
        }
            break;
        case sop::node::AttributeCreate::ItemType::Float:
        {
            switch (m_item_comp_sizes[i])
            {
            case 1:
                type = GeoAttrType::Float;
                break;
            case 2:
                type = GeoAttrType::Float2;
                break;
            case 3:
                type = GeoAttrType::Float3;
                break;
            case 4:
                type = GeoAttrType::Float4;
                break;
            }
        }
            break;
        case sop::node::AttributeCreate::ItemType::Vector:
            type = GeoAttrType::Vector;
            break;
        case sop::node::AttributeCreate::ItemType::String:
            type = GeoAttrType::String;
            break;
        default:
            // todo *Array
            assert(0);
        }
        m_geo_impl->GetAttr().AddAttr(m_item_classes[i], GeoAttribute::VarDesc(m_item_names[i], type), vars);
    }
}

bool AttributeCreate::IsNumberValid() const
{
    const size_t num = m_item_names.size();
    return m_item_classes.size() == num
        && m_item_types.size() == num
        && m_item_values.size() == num
        && m_item_default_vals.size() == num
        && m_item_flt_infos.size() == num
        && m_item_comp_sizes.size() == num
        && m_item_strings.size() == num
        ;
}

}
}