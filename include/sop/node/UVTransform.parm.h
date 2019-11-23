#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Translate, Vector3, t,     m_translate, (0.0f, 0.0f, 0.0f))
PARAM_INFO(Rotate,    Vector3, r,     m_rotate,    (0.0f, 0.0f, 0.0f))
PARAM_INFO(Scale,     Vector3, s,     m_scale,     (1.0f, 1.0f, 1.0f))
PARAM_INFO(Shear,     Vector3, shear, m_shear,     (0.0f, 0.0f, 0.0f))
