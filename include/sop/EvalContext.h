#pragma once

#include "sop/Node.h"
#include "sop/VarBuffer.h"

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
    int point_idx     = -1;
    //int vertex_idx    = -1;
    //int primitive_idx = -1;

    mutable VarBuffer var_buf;

}; // EvalContext

}