#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(SortKey,      Sort::Key, ptsort,       m_key,           (Sort::Key::NoChange))
PARAM_INFO(PointReverse, bool,      pointreverse, m_point_reverse, (false))
PARAM_INFO(PointOffset,  int,       pointoffset,  m_point_offset,  (0))