#ifndef PARM_FILEPATH
#error "You must define PARM_FILEPATH macro before include this file"
#endif

#ifndef PARM_NODE_CLASS
#error "You must define PARM_NODE_CLASS macro before include this file"
#endif

//#define GroupType  Int
//#define GroupMerge Int
//
//#define PARAM_INFO(id, type, name, member, default_val)                \
//.property(#name, &PARM_NODE_CLASS::Get##id, &PARM_NODE_CLASS::Set##id) \
//(                                                                      \
//    rttr::metadata(sop::PropMeta::ParmType, sop::ParmType::##type)     \
//)
//#include PARM_FILEPATH
//#undef  PARAM_INFO
//
//#undef GroupType
//#undef GroupMerge

#define PARAM_INFO(id, type, name, member, default_val)                \
.property(#name, &PARM_NODE_CLASS::Get##id, &PARM_NODE_CLASS::Set##id)
#include PARM_FILEPATH
#undef  PARAM_INFO