#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(PointsEnable, std::vector<bool>,     usept, m_points_enable, ())
PARAM_INFO(Points,       std::vector<sm::vec3>, pt,    m_points,        ())
