#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupName, std::string,                 group,         m_group_name, (""))
PARAM_INFO(Extrusion, PolyExtrude::Extrusion, extrusionmode, m_extrusion,  (PolyExtrude::Extrusion::PrimEdgeNorm))
PARAM_INFO(Distance,  float,                  dist,          m_distance,   (0.0f))

PARAM_INFO(OutputFront, bool, outputfront, m_output_front, (true))
PARAM_INFO(OutputBack,  bool, outputback,  m_output_back,  (false))
PARAM_INFO(OutputSide,  bool, outputside,  m_output_side,  (true))

PARAM_INFO(OutputFrontGroup, bool,    outputfrontgrp, m_output_front_group, (false))
PARAM_INFO(OutputBackGroup,  bool,    outputbackgrp,  m_output_back_group,  (false))
PARAM_INFO(OutputSideGroup,  bool,    outputsidegrp,  m_output_side_group,  (false))
PARAM_INFO(FrontGroupName,   std::string,  frontgrp,       m_front_group_name,   (""))
PARAM_INFO(BackGroupName,    std::string,  backgrp,        m_back_group_name,    (""))
PARAM_INFO(SideGroupName,    std::string,  sidegrp,        m_side_group_name,    (""))

PARAM_INFO(ThicknessRamp, std::vector<RampFloat>, thickness, m_thickness_ramp, ())
PARAM_INFO(TwistRamp,     std::vector<RampFloat>, twist,     m_twist_ramp,     ())
