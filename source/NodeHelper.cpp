#include "everything/NodeHelper.h"
#include "everything/Node.h"

namespace evt
{

bool NodeHelper::IsRoot(const Node& node)
{
    for (auto& in : node.GetImports()) {
        if (!in.conns.empty()) {
            return false;
        }
    }
    return true;
}

NodePtr NodeHelper::GetInputNode(const Node& node, size_t idx)
{
    auto& imports = node.GetImports();
    if (idx < 0 || idx >= imports.size()) {
        return nullptr;
    }

    auto& conns = imports[idx].conns;
    if (conns.empty()) {
        return nullptr;
    }

    assert(imports[idx].conns.size() == 1);
    return imports[idx].conns[0].node.lock();
}

std::shared_ptr<GeometryImpl>
NodeHelper::GetInputGeo(const Node& node, size_t idx)
{
    auto prev_node = GetInputNode(node, idx);
    if (prev_node) {
        return prev_node->GetGeometry();
    } else {
        return nullptr;
    }
}

bool NodeHelper::CheckPropsType(const Node& node, int begin, int end, VariableType type)
{
    auto& props = node.GetProps().GetProps();
    for (int i = begin; i < end; ++i) {
        if (props[i].Val().type != type) {
            return false;
        }
    }
    return true;
}

}