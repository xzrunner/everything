#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupName, String,    group,     m_group_name, (""))
PARAM_INFO(GroupType, GroupType, grouptype, m_group_type, (GroupType::GuessFromGroup))

PARAM_INFO(Translate, Float3, t,     m_translate, (0.0f, 0.0f, 0.0f))
PARAM_INFO(Rotate,    Float3, r,     m_rotate,    (0.0f, 0.0f, 0.0f))
PARAM_INFO(Scale,     Float3, s,     m_scale,     (1.0f, 1.0f, 1.0f))
PARAM_INFO(Shear,     Float3, shear, m_shear,     (0.0f, 0.0f, 0.0f))
