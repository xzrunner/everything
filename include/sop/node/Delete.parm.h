#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(NegateSelected, Delete::NegateSelected, negate,  m_del_selected, (Delete::NegateSelected::Delete))
PARAM_INFO(EntityType,     Delete::EntityType,     entity,  m_entity_type,  (Delete::EntityType::Points))
PARAM_INFO(OpRule,         Delete::OpRule,         groupop, m_operation,    (Delete::OpRule::Pattern))
PARAM_INFO(FilterExpr,     String,                 filter,  m_filter_expr,  (""))
