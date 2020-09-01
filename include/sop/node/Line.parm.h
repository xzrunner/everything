#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Origin,    sm::vec3,  origin, m_origin,    (0.0f, 0.0f, 0.0f))
PARAM_INFO(Direction, sm::vec3,  dir,    m_direction, (0.0f, 1.0f, 0.0f))
PARAM_INFO(Length,    float,   dist,   m_len,       (1.0f))
PARAM_INFO(Points,    int,     points, m_points,    (2))
