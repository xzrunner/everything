#pragma once

#include "sop/typedef.h"
#include "sop/Variable.h"
#include "sop/Group.h"

namespace sop
{

class GeometryImpl;
class Evaluator;

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