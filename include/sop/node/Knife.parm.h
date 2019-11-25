#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(KeepType, Knife::KeepType, knifeop, m_keep, (Knife::KeepType::KeepAll))

PARAM_INFO(Origin,    Float3, origin, m_ori, (0.0f, 0.0f, 0.0f))
PARAM_INFO(Direction, Float3, dir,    m_dir, (0.0f, 1.0f, 0.0f))
