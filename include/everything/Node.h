#pragma once

#include "everything/Variable.h"

#include <node0/typedef.h>

#include <rttr/registration>

namespace evt
{

class TreeContext;

class Node
{
public:
    Node() {}
//    Node(const std::string& name);

    virtual void Execute(TreeContext& ctx) = 0;
    virtual void UpdateContext(TreeContext& ctx) {}

    struct Port;
    void SetImports(const std::vector<Port>& imports) { m_imports = imports; }
    void SetExports(const std::vector<Port>& exports) { m_exports = exports; }

    auto& GetImports() const { return m_imports; }
    auto& GetExports() const { return m_exports; }

    n0::SceneNodePtr GetSceneNode() const { return m_scene_node; }

    bool IsDirty() const { return m_dirty; }
    void SetDirty(bool dirty) const { m_dirty = dirty; }

public:
    struct PortAddr
    {
        PortAddr() {}
        PortAddr(const std::shared_ptr<Node>& node, int idx)
            : node(node), idx(idx) {}
        PortAddr(const PortAddr& addr)
            : node(addr.node), idx(addr.idx) {}

        std::weak_ptr<Node> node;
        int idx = -1;
    };

    struct Port
    {
        Port() {}
        Port(const Variable& var)
            : var(var) {}

        Variable var;
        std::vector<PortAddr> conns;
    };

protected:
    std::vector<Port> m_imports, m_exports;

    n0::SceneNodePtr m_scene_node = nullptr;

private:
//    std::string m_name;

    mutable bool m_dirty = true;

    RTTR_ENABLE()

}; // Node

void make_connecting(const Node::PortAddr& from, const Node::PortAddr& to);
void disconnect(const Node::PortAddr& from, const Node::PortAddr& to);

}