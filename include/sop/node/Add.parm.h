#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(PointsEnable, Array<Bool>,   usept, m_points_enable, ())
PARAM_INFO(Points,       Array<Float3>, pt,    m_points,        ())
