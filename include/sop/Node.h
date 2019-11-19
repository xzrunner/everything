#pragma once

#include "sop/NodeVarType.h"
#include "sop/NodePropsMgr.h"

#include <hdiop/NodeImpl.h>
#include <node0/typedef.h>

namespace sop
{

class GeometryImpl;

class Node : public hdiop::NodeImpl<NodeVarType>
{
public:
    Node(size_t build_in_count = 0);

    virtual void Execute(Evaluator& eval) = 0;

    virtual void AddInputPorts(size_t num) {}

    virtual void SetParm(const std::string& key, const Variable& val)
    {
        auto type = get_type().get_name().to_string();
        printf("setparm %s, %s\n", type.c_str(), key.c_str());
    }

    std::shared_ptr<GeometryImpl> GetGeometry() const { return m_geo_impl; }

    bool IsDirty() const { return m_dirty; }
    void SetDirty(bool dirty) const { m_dirty = dirty; }

    void SetParent(const std::shared_ptr<Node>& node);
    auto GetParent() const { return m_parent.lock(); }

    auto& GetProps() const { return m_props; }

    auto GetLevel() const { return m_level; }

protected:
    std::shared_ptr<GeometryImpl> m_geo_impl = nullptr;

    NodePropsMgr m_props;

private:
    std::weak_ptr<Node> m_parent;

    size_t m_level = 0;

    mutable bool m_dirty = true;

    RTTR_ENABLE(hdiop::NodeImpl<NodeVarType>)

}; // Node

}