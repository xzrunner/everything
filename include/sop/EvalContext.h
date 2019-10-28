#pragma once

#include "sop/Node.h"
#include "sop/VarBuffer.h"
#include "sop/GeoAttrClass.h"

namespace sop
{

class Evaluator;

struct EvalContext
{
    EvalContext(const Evaluator& eval, const Node& node)
        : eval(&eval), node(&node)
    {
    }

    const Evaluator* eval = nullptr;

    const Node* node = nullptr;

    // geo context
    int          attr_idx  = -1;
    GeoAttrClass attr_type = GeoAttrClass::MaxTypeNum;

    mutable VarBuffer var_buf;

}; // EvalContext

}