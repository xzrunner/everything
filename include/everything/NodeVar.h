#pragma once

#include "everything/NodeVarType.h"

#include <string>

namespace evt
{

struct NodeVar
{
    NodeVarType type;
    std::string  name;

}; // NodeVar

}