#pragma once

#include "everything/typedef.h"

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

}; // NodeHelper

}