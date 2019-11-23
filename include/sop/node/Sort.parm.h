#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(SortKey,      Sort::Key, ptsort,       m_key,           (Sort::Key::NoChange))
PARAM_INFO(PointReverse, Boolean,   pointreverse, m_point_reverse, (false))
PARAM_INFO(PointOffset,  Integer,   pointoffset,  m_point_offset,  (0))