#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(ATreatAs,        Boolean::GeoType,  asurface,        m_a_type,             (Boolean::GeoType::Solid))
PARAM_INFO(BTreatAs,        Boolean::GeoType,  bsurface,        m_b_type,             (Boolean::GeoType::Solid))
PARAM_INFO(Operator,        Boolean::Operator, booleanop,       m_operator,           (Boolean::Operator::Intersect))
PARAM_INFO(SubtractChoices, Boolean::SubType,  subtractchoices, m_subtract_type,      (Boolean::SubType::AMinusB))
PARAM_INFO(UseABSeamEdges,  bool,              useabseamedges,  m_use_ab_seam_edges,  (false))
PARAM_INFO(ABSeamEdgesName, std::string,       abseamedges,     m_ab_seam_edges_name, (""))
