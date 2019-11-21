#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Size,   Vector3, size,  m_size,  (1, 1, 1))
PARAM_INFO(Center, Vector3, t,     m_pos,   (0, 0, 0))
PARAM_INFO(Scale,  Vector3, scale, m_scale, (1, 1, 1))
