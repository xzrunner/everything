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

}