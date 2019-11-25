#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(FuseOP, Fuse::Operator, switcher1, m_fuse_op, (Fuse::Operator::Consolidate))

PARAM_INFO(Distance,      Float, dist,             m_distance,        (0.001f))
PARAM_INFO(KeepUnusedPts, Bool,  keepunusedpoints, m_keep_unused_pts, (false))

PARAM_INFO(UpdatePtNormals, Bool, updatenml, m_update_pt_norm, (true))
