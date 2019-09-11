#pragma once

#include "everything/Node.h"

namespace evt
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

}; // EvalContext

}