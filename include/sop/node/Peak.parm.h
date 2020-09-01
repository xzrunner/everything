#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupName, std::string,    group,     m_group_name, (""))
PARAM_INFO(GroupType, GroupType, grouptype, m_group_type, (GroupType::GuessFromGroup))

PARAM_INFO(Distance,     float, dist,       m_dist,           (0.0f))
PARAM_INFO(UpdateNormal, bool,  updatenmls, m_update_normals, (true))
