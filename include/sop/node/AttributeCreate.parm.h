#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupName, std::string, group,     m_group_name, (""))
PARAM_INFO(GroupTypes, GroupTypes, grouptype, m_group_type, (GroupTypes::Auto))

PARAM_INFO(ItemNames,         std::vector<std::string>,                       name,     m_item_names,        ())
PARAM_INFO(ItemClasses,       std::vector<GeoAttrClass>,                 class,    m_item_classes,      ())
PARAM_INFO(ItemTypes,         std::vector<AttributeCreate::ItemType>,    type,     m_item_types,        ())
// float, Integer, Vector
PARAM_INFO(ItemValues,        std::vector<sm::vec4>,                       value,    m_item_values,       ())
// float, Integer, Vector
PARAM_INFO(ItemDefaultValues, std::vector<sm::vec4>,                       default,  m_item_default_vals, ())
// float
PARAM_INFO(ItemFloatInfos,    std::vector<AttributeCreate::ItemFltInfo>, typeinfo, m_item_flt_infos,    ())
// float, Integer, *std::vector
PARAM_INFO(ItemCompSize,      std::vector<int>,                          size,     m_item_comp_sizes,   ())
// std::string
PARAM_INFO(ItemStrings,       std::vector<std::string>,                       string,   m_item_strings,      ())
