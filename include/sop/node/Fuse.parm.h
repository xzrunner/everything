#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Distance,      Float,   dist,             m_distance,        (0.001f))
PARAM_INFO(KeepUnusedPts, Boolean, keepunusedpoints, m_keep_unused_pts, (false))

PARAM_INFO(UpdatePtNormals, Boolean, updatenml, m_update_pt_norm, (true))
