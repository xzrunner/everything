#include "sop/node/AttributeCreate.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/ParmList.h"

namespace sop
{
namespace node
{

void AttributeCreate::Execute(const ur2::Device& dev, Evaluator& eval)
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
        ParmType type;
        switch (m_item_types[i])
        {
        case sop::node::AttributeCreate::ItemType::Integer:
        {
            switch (m_item_comp_sizes[i])
            {
            case 1:
                type = ParmType::Int;
                break;
            case 2:
                type = ParmType::Int2;
                break;
            case 3:
                type = ParmType::Int3;
                break;
            case 4:
                type = ParmType::Int4;
                break;
            }
        }
            break;
        case sop::node::AttributeCreate::ItemType::Float:
        {
            switch (m_item_comp_sizes[i])
            {
            case 1:
                type = ParmType::Float;
                break;
            case 2:
                type = ParmType::Float2;
                break;
            case 3:
                type = ParmType::Float3;
                break;
            case 4:
                type = ParmType::Float4;
                break;
            }
        }
            break;
        case sop::node::AttributeCreate::ItemType::Vector:
            type = ParmType::Vector;
            break;
        case sop::node::AttributeCreate::ItemType::String:
            type = ParmType::String;
            break;
        default:
            // todo *Array
            assert(0);
        }

        const size_t num = m_geo_impl->GetAttr().GetSize(m_item_classes[i]);

        // int
        if (m_item_types[i] == ItemType::Integer && m_item_comp_sizes[i] == 1)
        {
            std::vector<int> data;
            if (group)
            {
                data.resize(num, static_cast<int>(m_item_default_vals[i].x));
                for (auto& i : group->GetItems()) {
                    data[i] = static_cast<int>(m_item_values[i].x);
                }
            }
            else
            {
                data.resize(num, static_cast<int>(m_item_values[i].x));
            }
            m_geo_impl->GetAttr().AddParmList(m_item_classes[i],
                std::make_shared<ParmIntList>(m_item_names[i], type, data)
            );
        }
        // float
        else if (m_item_types[i] == ItemType::Float && m_item_comp_sizes[i] == 1)
        {
            std::vector<float> data;
            if (group)
            {
                data.resize(num, static_cast<float>(m_item_default_vals[i].x));
                for (auto& i : group->GetItems()) {
                    data[i] = static_cast<float>(m_item_values[i].x);
                }
            }
            else
            {
                data.resize(num, static_cast<float>(m_item_values[i].x));
            }
            m_geo_impl->GetAttr().AddParmList(m_item_classes[i],
                std::make_shared<ParmFltList>(m_item_names[i], type, data)
            );
        }
        // float3
        else if ((m_item_types[i] == ItemType::Float || m_item_types[i] == ItemType::Vector)
              && m_item_comp_sizes[i] == 3)
        {
            std::vector<sm::vec3> data;
            if (group)
            {
                data.resize(num, static_cast<sm::vec3>(m_item_default_vals[i].xyzw));
                for (auto& i : group->GetItems()) {
                    data[i] = static_cast<sm::vec3>(m_item_values[i].xyzw);
                }
            }
            else
            {
                data.resize(num, static_cast<sm::vec3>(m_item_values[i].xyzw));
            }
            m_geo_impl->GetAttr().AddParmList(m_item_classes[i],
                std::make_shared<ParmFlt3List>(m_item_names[i], type, data)
            );
        }
        // string
        else if (m_item_types[i] == ItemType::String)
        {
            std::vector<std::string> data;
            if (group)
            {
                data.resize(num, std::string());
                for (auto& i : group->GetItems()) {
                    data[i] = m_item_strings[i];
                }
            }
            else
            {
                data.resize(num, m_item_strings[i]);
            }
            m_geo_impl->GetAttr().AddParmList(m_item_classes[i],
                std::make_shared<ParmStrList>(m_item_names[i], type, data)
            );
        }
        // todo
        else
        {
            assert(0);
        }
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