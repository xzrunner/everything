#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(OriName, std::string,   inname,   m_attr_name, (""))
PARAM_INFO(OriClass, GeoAttrClass, inclass,  m_from_type, (GeoAttrClass::Point))
PARAM_INFO(NewClass, GeoAttrClass, outclass, m_to_type,   (GeoAttrClass::Point))
