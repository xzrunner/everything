#pragma once

#include "everything/NodeVar.h"
#include "everything/Variable.h"
#include "everything/NodePropsMgr.h"

#include <node0/typedef.h>

#include <rttr/registration>

namespace evt
{

class GeometryImpl;

class Node
{
public:
    Node(size_t build_in_count = 0);

    virtual void Execute(Evaluator& eval) = 0;

    virtual void AddInputPorts(size_t num) {}

    struct Port;
    void SetImports(const std::vector<Port>& imports) { m_imports = imports; }
    void SetExports(const std::vector<Port>& exports) { m_exports = exports; }

    auto& GetImports() const { return m_imports; }
    auto& GetExports() const { return m_exports; }

    std::shared_ptr<GeometryImpl> GetGeometry() const { return m_geo_impl; }

    bool IsDirty() const { return m_dirty; }
    void SetDirty(bool dirty) const { m_dirty = dirty; }

    void SetParent(const std::shared_ptr<Node>& node);
    auto GetParent() const { return m_parent.lock(); }

    auto& GetProps() const { return m_props; }

    auto GetLevel() const { return m_level; }

    void SetName(const std::string& name) { m_name = name; }
    auto& GetName() const { return m_name; }

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
        Port(const NodeVar& var)
            : var(var) {}

        NodeVar var;
        std::vector<PortAddr> conns;
    };

protected:
    std::vector<Port> m_imports, m_exports;

    std::shared_ptr<GeometryImpl> m_geo_impl = nullptr;

    NodePropsMgr m_props;

private:
    std::string m_name;

    std::weak_ptr<Node> m_parent;

    size_t m_level = 0;

    mutable bool m_dirty = true;

    RTTR_ENABLE()

}; // Node

void make_connecting(const Node::PortAddr& from, const Node::PortAddr& to);
void disconnect(const Node::PortAddr& from, const Node::PortAddr& to);

}