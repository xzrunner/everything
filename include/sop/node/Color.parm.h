#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(AttrAddTo, GeoAttrClass,     class, m_attr_add_to, (GeoAttrClass::Point))
PARAM_INFO(Color,     Float3,           color, m_color,       (1.0f, 1.0f, 1.0f))
PARAM_INFO(Ramp,      Array<RampColor>, ramp,  m_ramp,        ())
