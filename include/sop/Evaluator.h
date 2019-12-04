#pragma once

#include "sop/typedef.h"
#include "sop/Node.h"

#include <set>
#include <vector>

namespace sop
{

struct EvalContext;

class Evaluator
{
public:
    // update node
    void NodeChaged(const NodePtr& node);

    // update nodes
    void AddNode(const NodePtr& node);
    void RemoveNode(const NodePtr& node);
    void ClearAllNodes();

    // update node prop
    void PropChanged(const NodePtr& node);

    // update node conn
    void Connect(const dag::Node<NodeVarType>::PortAddr& from, const dag::Node<NodeVarType>::PortAddr& to);
    void Disconnect(const dag::Node<NodeVarType>::PortAddr& from, const dag::Node<NodeVarType>::PortAddr& to);
    void RebuildConnections(const std::vector<std::pair<dag::Node<NodeVarType>::PortAddr, dag::Node<NodeVarType>::PortAddr>>& conns);

    void Update(bool force = false);

    void MakeDirty(bool all_nodes_dirty = true);

    dag::Variable CalcExpr(const std::string& expr, const EvalContext& ctx) const;
    void RunStatement(const std::string& stmt, const EvalContext& ctx) const;

    void Rename(const std::string& from, const std::string& to);

    NodePtr QueryNodeByName(const std::string& name) const;
    NodePtr QueryNodeByPath(const std::string& path) const;
    const Node* QueryNodeByPath(const Node* base, const std::string& path) const;

private:
    void UpdateProps();
    void UpdateNodes();

    void TopologicalSorting();

    static void SetTreeDirty(const NodePtr& root);

    static bool HasNodeConns(const NodePtr& node);

private:
    std::map<std::string, NodePtr> m_nodes_map;

    std::vector<NodePtr> m_nodes_sorted;

    bool m_dirty = false;

    size_t m_next_id = 0;

}; // Evaluator

}
