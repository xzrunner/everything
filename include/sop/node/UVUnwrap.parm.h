#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Scale,  UVUnwrap::ScaleType, scale, m_scale,  (UVUnwrap::ScaleType::Uniform))
PARAM_INFO(Rotate, Vector3,             r,     m_rotate, (0.0f, 0.0f, 0.0f))
