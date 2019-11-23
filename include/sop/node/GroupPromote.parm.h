#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupNames,    Array<String>,     group,    m_group_names, ())
PARAM_INFO(SrcGroupTypes, Array<GroupTypes>, fromtype, m_src_types,   ())
PARAM_INFO(DstGroupTypes, Array<GroupTypes>, totype,   m_dst_types,   ())
