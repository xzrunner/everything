#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupName, String,                 group,         m_group_name, (""))
PARAM_INFO(Extrusion, PolyExtrude::Extrusion, extrusionmode, m_extrusion,  (PolyExtrude::Extrusion::PrimEdgeNorm))
PARAM_INFO(Distance,  Float,                  dist,          m_distance,   (0.0f))

PARAM_INFO(OutputFront, Bool, outputfront, m_output_front, (true))
PARAM_INFO(OutputBack,  Bool, outputback,  m_output_back,  (false))
PARAM_INFO(OutputSide,  Bool, outputside,  m_output_side,  (true))

PARAM_INFO(OutputFrontGroup, Bool,    outputfrontgrp, m_output_front_group, (false))
PARAM_INFO(OutputBackGroup,  Bool,    outputbackgrp,  m_output_back_group,  (false))
PARAM_INFO(OutputSideGroup,  Bool,    outputsidegrp,  m_output_side_group,  (false))
PARAM_INFO(FrontGroupName,   String,  frontgrp,       m_front_group_name,   (""))
PARAM_INFO(BackGroupName,    String,  backgrp,        m_back_group_name,    (""))
PARAM_INFO(SideGroupName,    String,  sidegrp,        m_side_group_name,    (""))

PARAM_INFO(ThicknessRamp, Array<RampFloat>, thickness, m_thickness_ramp, ())
PARAM_INFO(TwistRamp,     Array<RampFloat>, twist,     m_twist_ramp,     ())
