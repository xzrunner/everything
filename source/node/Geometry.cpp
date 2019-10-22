#include "sop/node/Geometry.h"
#include "sop/NodeHelper.h"
#include "sop/node/Output.h"

namespace sop
{
namespace node
{

void Geometry::Execute(Evaluator& eval)
{
    for (auto& c : m_children) {
        if (NodeHelper::IsRoot(*c)) {
            c->Execute(eval);
        }
    }
}

void Geometry::AddChild(const std::shared_ptr<Geometry>& parent,
                        const std::shared_ptr<Node>& child)
{
    if (!parent || !child) {
        return;
    }
    for (auto& c : parent->m_children) {
        if (c == child) {
            return;
        }
    }

    parent->m_children.push_back(child);
    child->SetParent(parent);
}

std::shared_ptr<Node>
Geometry::QueryChild(const std::string& name) const
{
    for (auto& c : m_children) {
        if (c->GetName() == name) {
            return c;
        }
    }
    return nullptr;
}

std::shared_ptr<Node> 
Geometry::QueryOutput(int idx) const
{
    for (auto& c : m_children) 
    {
        if (c->get_type() == rttr::type::get<node::Output>()) {
            auto output = std::static_pointer_cast<node::Output>(c);
            if (output->GetOutputIdx() == idx) {
                return c;
            }
        }
    }
    return nullptr;
}

}
}