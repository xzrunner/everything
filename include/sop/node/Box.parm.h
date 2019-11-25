#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Size,   Float3, size,  m_size,  (1.0f, 1.0f, 1.0f))
PARAM_INFO(Center, Float3, t,     m_pos,   (0.0f, 0.0f, 0.0f))
PARAM_INFO(Scale,  Float3, scale, m_scale, (1.0f, 1.0f, 1.0f))

PARAM_INFO(Divisions, Bool, dodivs, m_divisions, (false))
