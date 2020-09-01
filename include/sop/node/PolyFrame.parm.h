#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(GroupName,  std::string,                group,  m_group_name,  (""))
PARAM_INFO(EntityType, GroupType,             entity, m_entity_type, (GroupType::Primitives))
PARAM_INFO(FrameStyle, PolyFrame::FrameStyle, style,  m_frame_style, (FrameStyle::TwoEdges))

PARAM_INFO(NormalNameToggle,    bool,         Non,        m_normal_name_toggle,    (true))
PARAM_INFO(TangentNameToggle,   bool,         tangentuon, m_tangent_name_toggle,   (true))
PARAM_INFO(BitangentNameToggle, bool,         tangentvon, m_bitangent_name_toggle, (false))
PARAM_INFO(NormalName,          std::string,  N,          m_normal_name,           ("N"))
PARAM_INFO(TangentName,         std::string,  tangentu,   m_tangent_name,          ("tangentu"))
PARAM_INFO(BitangentName,       std::string,  tangentv,   m_bitangent_name,        ("N"))
