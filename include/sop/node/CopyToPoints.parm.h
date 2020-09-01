#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(SourceGroup,  std::string, sourcegroup, m_source_group, (""))
PARAM_INFO(TargetPoints, std::string, targetgroup, m_target_group, (""))

PARAM_INFO(ShowGuide,        bool, showguide, m_show_guide,       (false))
PARAM_INFO(TransUsePointDir, bool, transform, m_trans_use_pt_dir, (false))
PARAM_INFO(CopyPointAttr,    bool, doattr,    m_copy_pt_attr,     (false))
