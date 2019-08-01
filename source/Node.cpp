#include "everything/Node.h"

#include <assert.h>

RTTR_REGISTRATION
{

rttr::registration::class_<evt::Node::Port>("evt::Node::Port")
	.property("var", &evt::Node::Port::var)
;

rttr::registration::class_<evt::Node>("evt::Node")
	.method("GetImports", &evt::Node::GetImports)
	.method("GetExports", &evt::Node::GetExports)
;

}

namespace evt
{

//Node::Node(const std::string& name)
//    : m_name(name)
//{
//}

void Node::UpdateContext(TreeContext& ctx)
{
    UpdateCtxSelf(ctx);

    for (auto& o : m_exports) {
        for (auto& c : o.conns) {
            auto n = c.node.lock();
            if (n) {
                n->UpdateContext(ctx);
            }
        }
    }
}

void Node::Execute(bool only_self)
{
    ExecuteSelf();
    if (!only_self) {
        ExecuteExports();
    }
}

void Node::ExecuteExports()
{
    for (auto& o : m_exports) {
        for (auto& c : o.conns) {
            auto n = c.node.lock();
            if (n) {
                n->Execute(false);
            }
        }
    }
}

void make_connecting(const Node::PortAddr& from, const Node::PortAddr& to)
{
	{
		auto node = from.node.lock();
		assert(node);
		auto& ports = node->GetExports();
		assert(from.idx >= 0 && from.idx < static_cast<int>(ports.size()));
		const_cast<Node::Port&>(ports[from.idx]).conns.push_back(to);
	}
	{
		auto node = to.node.lock();
		assert(node);
		auto& ports = node->GetImports();
		assert(to.idx >= 0 && to.idx < static_cast<int>(ports.size())
            && ports[to.idx].conns.empty());
		const_cast<Node::Port&>(ports[to.idx]).conns.push_back(from);
	}
}

void disconnect(const Node::PortAddr& from, const Node::PortAddr& to)
{
    auto f_node = from.node.lock();
    auto t_node = to.node.lock();
    if (!f_node || !t_node) {
        return;
    }

    auto& f_port = f_node->GetExports()[from.idx];
    bool finded = false;
    for (auto itr = f_port.conns.begin(); itr != f_port.conns.end(); )
    {
        if (itr->node.lock() == t_node && itr->idx == to.idx) {
            itr = const_cast<Node::Port&>(f_port).conns.erase(itr);
            finded = true;
        } else {
            ++itr;
        }
    }
    assert(finded);

    auto& t_port = t_node->GetImports()[to.idx];
    finded = false;
    for (auto itr = t_port.conns.begin(); itr != t_port.conns.end(); )
    {
        if (itr->node.lock() == f_node && itr->idx == from.idx) {
            itr = const_cast<Node::Port&>(t_port).conns.erase(itr);
            finded = true;
        } else {
            ++itr;
        }
    }
    assert(finded);
}

}