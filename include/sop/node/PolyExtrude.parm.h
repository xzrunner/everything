#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupName, String,                 group,         m_group_name, (""))
PARAM_INFO(Extrusion, PolyExtrude::Extrusion, extrusionmode, m_extrusion,  (PolyExtrude::Extrusion::PrimEdgeNorm))
PARAM_INFO(Distance,  Float,                  dist,          m_distance,   (0.0f))

PARAM_INFO(OutputFront, Boolean, outputfront, m_output_front, (true))
PARAM_INFO(OutputBack,  Boolean, outputback,  m_output_back,  (false))
PARAM_INFO(OutputSide,  Boolean, outputside,  m_output_side,  (true))

PARAM_INFO(OutputFrontGroup, Boolean, outputfrontgrp, m_output_front_group, (false))
PARAM_INFO(OutputBackGroup,  Boolean, outputbackgrp,  m_output_back_group,  (false))
PARAM_INFO(OutputSideGroup,  Boolean, outputsidegrp,  m_output_side_group,  (false))
PARAM_INFO(FrontGroupName,   String,  frontgrp,       m_front_group_name,   (""))
PARAM_INFO(BackGroupName,    String,  backgrp,        m_back_group_name,    (""))
PARAM_INFO(SideGroupName,    String,  sidegrp,        m_side_group_name,    (""))

PARAM_INFO(ThicknessRamp, Array<RampWidgets>, thickness, m_thickness_ramp, ())
PARAM_INFO(TwistRamp,     Array<RampWidgets>, twist,     m_twist_ramp,     ())
