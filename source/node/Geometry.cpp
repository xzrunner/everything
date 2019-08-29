#include "everything/node/Geometry.h"
#include "everything/NodeHelper.h"

namespace evt
{
namespace node
{

void Geometry::Execute(TreeContext& ctx)
{
    for (auto& c : m_children) {
        if (NodeHelper::IsRoot(*c)) {
            c->Execute(ctx);
        }
    }
}

void Geometry::AddChild(const std::shared_ptr<Geometry>& parent,
                        const std::shared_ptr<Node>& child)
{
    if (!parent || !child) {
        return;
    }
    if (parent->m_children.find(child) != parent->m_children.end()) {
        return;
    }

    parent->m_children.insert(child);
    child->SetParent(parent);
}

}
}