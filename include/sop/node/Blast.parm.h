#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupName, std::string, group,     m_group_name, (""))
PARAM_INFO(GroupType, GroupType,   grouptype, m_group_type, (GroupType::GuessFromGroup))

PARAM_INFO(RecomputeNormals, bool, computenorms, m_recompute_normals, (false))
PARAM_INFO(DelNonSelected,   bool, negate,       m_del_non_selected,  (false))
PARAM_INFO(DelUnusedGroups,  bool, removegrp,    m_del_unused_groups, (false))
