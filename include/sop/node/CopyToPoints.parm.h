#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(SourceGroup,  String, sourcegroup, m_source_group, (""))
PARAM_INFO(TargetPoints, String, targetgroup, m_target_group, (""))

PARAM_INFO(ShowGuide,        Boolean, showguide, m_show_guide,       (false))
PARAM_INFO(TransUsePointDir, Boolean, transform, m_trans_use_pt_dir, (false))
PARAM_INFO(CopyPointAttr,    Boolean, doattr,    m_copy_pt_attr,     (false))
