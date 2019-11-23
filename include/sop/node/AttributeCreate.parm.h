#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupName,  String,     group,     m_group_name, (""))
PARAM_INFO(GroupTypes, GroupTypes, grouptype, m_group_type, (GroupTypes::Auto))

PARAM_INFO(ItemNames,         Array<String>,                       name,     m_item_names,        ())
PARAM_INFO(ItemClasses,       Array<GeoAttrClass>,                 class,    m_item_classes,      ())
PARAM_INFO(ItemTypes,         Array<AttributeCreate::ItemType>,    type,     m_item_types,        ())
// Float, Integer, Vector
PARAM_INFO(ItemValues,        Array<Vector4>,                      value,    m_item_values,       ())
// Float, Integer, Vector
PARAM_INFO(ItemDefaultValues, Array<Vector4>,                      default,  m_item_default_vals, ())
// Float
PARAM_INFO(ItemFloatInfos,    Array<AttributeCreate::ItemFltInfo>, typeinfo, m_item_flt_infos,    ())
// Float, Integer, *Array
PARAM_INFO(ItemCompSize,      Array<Integer>,                      size,     m_item_comp_sizes,   ())
// String
PARAM_INFO(ItemStrings,       Array<String>,                       string,   m_item_strings,      ())
