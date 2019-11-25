#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupName,  String,                group,  m_group_name,  (""))
PARAM_INFO(EntityType, GroupType,             entity, m_entity_type, (GroupType::Primitives))
PARAM_INFO(FrameStyle, PolyFrame::FrameStyle, style,  m_frame_style, (FrameStyle::TwoEdges))

PARAM_INFO(NormalNameToggle,    Bool,    Non,        m_normal_name_toggle,    (true))
PARAM_INFO(TangentNameToggle,   Bool,    tangentuon, m_tangent_name_toggle,   (true))
PARAM_INFO(BitangentNameToggle, Bool,    tangentvon, m_bitangent_name_toggle, (false))
PARAM_INFO(NormalName,          String,  N,          m_normal_name,           ("N"))
PARAM_INFO(TangentName,         String,  tangentu,   m_tangent_name,          ("tangentu"))
PARAM_INFO(BitangentName,       String,  tangentv,   m_bitangent_name,        ("N"))
