#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupName, std::string, group, m_group_name, (""))

PARAM_INFO(DoTransformation, bool, doxform, m_do_trans, (false))

PARAM_INFO(Translate, sm::vec3, t,     m_translate, (0.0f, 0.0f, 0.0f))
PARAM_INFO(Rotate,    sm::vec3, r,     m_rotate,    (0.0f, 0.0f, 0.0f))
PARAM_INFO(Scale,     sm::vec3, s,     m_scale,     (1.0f, 1.0f, 1.0f))
PARAM_INFO(Shear,     sm::vec3, shear, m_shear,     (0.0f, 0.0f, 0.0f))

PARAM_INFO(VectorAttrToTrans, std::string, xformattribs, m_trans_attrs, (""))
