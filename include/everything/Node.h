#pragma once

#include "everything/NodeVar.h"
#include "everything/Variable.h"

#include <node0/typedef.h>

#include <rttr/registration>

namespace evt
{

class TreeContext;
class GeometryImpl;

class Node
{
public:
    Node() {}
//    Node(const std::string& name);

    virtual void Execute(TreeContext& ctx) = 0;
    virtual void UpdateContext(TreeContext& ctx) {}

    virtual void AddInputPorts(size_t num) {}

    Variable QueryProperty(const std::string& key) const;
    void AddProperty(const std::string& key, const Variable& val);
    void RemoveProperty(const std::string& key);
    void ClearProperty();

    struct Port;
    void SetImports(const std::vector<Port>& imports) { m_imports = imports; }
    void SetExports(const std::vector<Port>& exports) { m_exports = exports; }

    auto& GetImports() const { return m_imports; }
    auto& GetExports() const { return m_exports; }

    std::shared_ptr<GeometryImpl> GetGeometry() const { return m_geo_impl; }

    bool IsDirty() const { return m_dirty; }
    void SetDirty(bool dirty) const { m_dirty = dirty; }

    std::shared_ptr<Geometry> GetInputGeo(size_t idx) const;

private:
    virtual Variable QueryBuildInProp(const std::string& key) const { return Variable(); }

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


private:
//    std::string m_name;

    mutable bool m_dirty = true;

    std::map<std::string, Variable> m_props;

    RTTR_ENABLE()

}; // Node

void make_connecting(const Node::PortAddr& from, const Node::PortAddr& to);
void disconnect(const Node::PortAddr& from, const Node::PortAddr& to);

}