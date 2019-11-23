#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupName,  String,     groupname, m_group_name,  (""))
PARAM_INFO(GroupType,  GroupType,  grouptype, m_group_type,  (GroupType::Primitives))
PARAM_INFO(GroupMerge, GroupMerge, mergeop,   m_group_merge, (GroupMerge::Replace))

PARAM_INFO(BaseGroupEnable, Boolean, groupbase, m_base_group_enable, (false))
PARAM_INFO(BaseGroupSyntax, String,  basegroup, m_base_group_syntax, (""))

PARAM_INFO(KeepInBounding, Boolean,                   groupbounding, m_keep_in_bounding, (false))
PARAM_INFO(BoundingType,   GroupCreate::BoundingType, boundtype,     m_bounding_type,    (GroupCreate::BoundingType::Box))

PARAM_INFO(KeepByNormals,      Boolean, groupnormal, m_keep_by_normals,       (false))
PARAM_INFO(KeepByNormalsDir,   Vector3, dir,         m_keep_by_normals_dir,   (0.0f, 0.0f, 1.0f))
PARAM_INFO(KeepByNormalsAngle, Float,   angle,       m_keep_by_normals_angle, (180.0f))
