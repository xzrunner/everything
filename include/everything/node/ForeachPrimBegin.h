#pragma once

#include "everything/Node.h"

namespace evt
{
namespace node
{

class ForeachPrimBegin : public Node
{
public:
    ForeachPrimBegin();

    virtual void Execute(Evaluator& eval, TreeContext& ctx) override {}

    RTTR_ENABLE(Node)

}; // ForeachPrimBegin

}
}