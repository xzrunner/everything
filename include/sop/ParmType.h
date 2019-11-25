#pragma once

namespace sop
{

enum class ParmType
{
    Unsupported,

#define PARM_INFO(type, name, label, size) type,
#include "sop/parm_cfg.h"
#undef PARM_INFO

    Array,
};

}