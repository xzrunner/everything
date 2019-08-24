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

    virtual void Execute(TreeContext& ctx) override {}

    RTTR_ENABLE(Node)

}; // ForeachPrimBegin

}
}