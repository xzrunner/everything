#pragma once

#include "everything/VariableType.h"

#include <string>

namespace evt
{

struct Variable
{
    VariableType type;
    std::string  name;

}; // Variable

}