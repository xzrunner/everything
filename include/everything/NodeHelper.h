#pragma once

#include "everything/typedef.h"
#include "everything/Variable.h"
#include "everything/Group.h"

namespace evt
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

    // begin <= i < end
    static bool CheckPropsType(const Node& node, int begin, int end, VariableType type);

    static std::vector<size_t> SelectGeoByExpr(GroupType type, const Evaluator& eval, 
        const Node& node, const std::string& expr);

}; // NodeHelper

}