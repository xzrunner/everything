#pragma once

#include "sop/NodeVarType.h"
#include "sop/NodeParmsMgr.h"

#include <dag/Node.h>
#include <node0/typedef.h>

namespace ur2 { class Device; }

namespace sop
{

class GeometryImpl;
class Volume;

class Node : public dag::Node<NodeVarType>
{
public:
    Node();

    virtual void Execute(const ur2::Device& dev, Evaluator& eval) = 0;

    virtual void AddInputPorts(size_t num) {}

    auto GetGeometry() const { return m_geo_impl; }
    auto GetVolume() const { return m_volume; }

    void SetParent(const std::shared_ptr<Node>& node);
    auto GetParent() const { return m_parent.lock(); }

    auto& GetParms() const { return m_parms; }

    auto GetLevel() const { return m_level; }

protected:
    std::shared_ptr<GeometryImpl> m_geo_impl = nullptr;
    std::shared_ptr<Volume>       m_volume   = nullptr;

    NodeParmsMgr m_parms;

private:
    std::weak_ptr<Node> m_parent;

    size_t m_level = 0;

    RTTR_ENABLE(dag::Node<NodeVarType>)

}; // Node

}