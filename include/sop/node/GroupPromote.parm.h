#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupNames,    std::vector<std::string>,     group,    m_group_names, ())
PARAM_INFO(SrcGroupTypes, std::vector<GroupTypes>, fromtype, m_src_types,   ())
PARAM_INFO(DstGroupTypes, std::vector<GroupTypes>, totype,   m_dst_types,   ())
