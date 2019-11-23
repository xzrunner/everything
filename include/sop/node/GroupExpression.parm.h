#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupType, GroupType, grouptype, m_group_type, (GroupType::Primitives))

PARAM_INFO(GroupNames,    Array<String>,     groupname, m_group_names,     ())
PARAM_INFO(GroupExprs,    Array<String>,     snippet,   m_group_exprs,     ())
PARAM_INFO(GroupMergeOps, Array<GroupMerge>, mergeop,   m_group_merge_ops, ())
