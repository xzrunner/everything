#pragma once

#include "sop/NodeVarType.h"

#include <string>

namespace sop
{

struct NodeVar
{
    NodeVarType type;
    std::string  name;

}; // NodeVar

}