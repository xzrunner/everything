#pragma once

#include "sop/typedef.h"
#include "sop/Variable.h"
#include "sop/Group.h"

namespace sop
{

class GeometryImpl;
class Evaluator;

#define NODE_PROP_SET(prop, val) \
    if (prop != val) {           \
        prop = val;              \
        SetDirty(true);          \
    }                            \

class NodeHelper
{
public:
    static bool IsRoot(const Node& node);

    static NodePtr GetInputNode(const Node& node, size_t idx);
    static std::shared_ptr<GeometryImpl>
        GetInputGeo(const Node& node, size_t idx);

    static std::vector<size_t> SelectGeoByExpr(GroupType type, const Evaluator& eval,
        const Node& node, const std::string& expr);

}; // NodeHelper

}