#pragma once

#include "everything/typedef.h"
#include "everything/Variable.h"

namespace evt
{

class GeometryImpl;

class NodeHelper
{
public:
    static bool IsRoot(const Node& node);

    static NodePtr GetInputNode(const Node& node, size_t idx);
    static std::shared_ptr<GeometryImpl>
        GetInputGeo(const Node& node, size_t idx);

    // begin <= i < end
    static bool CheckPropsType(const Node& node, int begin, int end, VariableType type);

}; // NodeHelper

}