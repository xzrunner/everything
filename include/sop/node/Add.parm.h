#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(PointsEnable, Array<Boolean>, usept, m_points_enable, ())
PARAM_INFO(Points,       Array<Vector3>, pt,    m_points,        ())
