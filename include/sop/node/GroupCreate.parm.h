#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupName,  std::string,     groupname, m_group_name,  (""))
PARAM_INFO(GroupType,  GroupType,  grouptype, m_group_type,  (GroupType::Primitives))
PARAM_INFO(GroupMerge, GroupMerge, mergeop,   m_group_merge, (GroupMerge::Replace))

PARAM_INFO(BaseGroupEnable, bool,    groupbase, m_base_group_enable, (false))
PARAM_INFO(BaseGroupSyntax, std::string,  basegroup, m_base_group_syntax, (""))

PARAM_INFO(KeepInBounding, bool,                      groupbounding, m_keep_in_bounding, (false))
PARAM_INFO(BoundingType,   GroupCreate::BoundingType, boundtype,     m_bounding_type,    (GroupCreate::BoundingType::Box))

PARAM_INFO(KeepByNormals,      bool,   groupnormal, m_keep_by_normals,       (false))
PARAM_INFO(KeepByNormalsDir,   sm::vec3, dir,         m_keep_by_normals_dir,   (0.0f, 0.0f, 1.0f))
PARAM_INFO(KeepByNormalsAngle, float,  angle,       m_keep_by_normals_angle, (180.0f))
