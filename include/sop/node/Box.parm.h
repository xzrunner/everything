#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Size,   sm::vec3, size,  m_size,  (1.0f, 1.0f, 1.0f))
PARAM_INFO(Center, sm::vec3, t,     m_pos,   (0.0f, 0.0f, 0.0f))
PARAM_INFO(Scale,  sm::vec3, scale, m_scale, (1.0f, 1.0f, 1.0f))

PARAM_INFO(Divisions, bool, dodivs, m_divisions, (false))
