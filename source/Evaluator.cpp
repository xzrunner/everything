#include "sop/Evaluator.h"
#include "sop/EvalContext.h"
#include "sop/node/Subnetwork.h"
#include "sop/node/ObjectMerge.h"

#include <cslang/EvalAST.h>
#include <cslang/Parser.h>
#include <cpputil/StringHelper.h>
#include <dag/Graph.h>

#include <stack>
#include <queue>

#include <assert.h>

namespace sop
{

void Evaluator::NodeChaged(const NodePtr& node)
{
    dag::Graph<NodeVarType>::SetTreeDirty(node);

    m_dirty = true;
}

void Evaluator::AddNode(const NodePtr& node)
{
    auto name = node->GetName();
    while (name.empty() || m_nodes_map.find(name) != m_nodes_map.end())
    {
        if (node->GetName().empty()) {
            name = "node" + std::to_string(m_next_id++);
        } else {
            name = node->GetName() + std::to_string(m_next_id++);
        }
    }
    node->SetName(name);

    assert(m_nodes_map.size() == m_nodes_sorted.size());
    m_nodes_map.insert({ name, node });
    m_nodes_sorted.insert(m_nodes_sorted.begin(), node);

    m_dirty = true;
}

void Evaluator::RemoveNode(const NodePtr& node)
{
    if (m_nodes_map.empty()) {
        return;
    }

    auto itr = m_nodes_map.find(node->GetName());
    if (itr == m_nodes_map.end()) {
        return;
    }

    dag::Graph<NodeVarType>::SetTreeDirty(node);

    assert(m_nodes_map.size() == m_nodes_sorted.size());
    m_nodes_map.erase(itr);
    for (auto itr = m_nodes_sorted.begin(); itr != m_nodes_sorted.end(); ++itr) {
        if (*itr == node) {
            m_nodes_sorted.erase(itr);
            break;
        }
    }

    m_dirty = true;
}

void Evaluator::ClearAllNodes()
{
    if (m_nodes_map.empty()) {
        return;
    }

    assert(m_nodes_map.size() == m_nodes_sorted.size());
    m_nodes_map.clear();
    m_nodes_sorted.clear();

    m_dirty = true;
}

void Evaluator::PropChanged(const NodePtr& node)
{
    dag::Graph<NodeVarType>::SetTreeDirty(node);

    m_dirty = true;
}

void Evaluator::Connect(const dag::Node<NodeVarType>::PortAddr& from, const dag::Node<NodeVarType>::PortAddr& to)
{
    dag::make_connecting<NodeVarType>(from, to);

    auto node = to.node.lock();
    assert(node && node->get_type().is_derived_from<Node>());
    dag::Graph<NodeVarType>::SetTreeDirty(std::static_pointer_cast<Node>(node));

    m_dirty = true;
}

void Evaluator::Disconnect(const dag::Node<NodeVarType>::PortAddr& from, const dag::Node<NodeVarType>::PortAddr& to)
{
    dag::disconnect<NodeVarType>(from, to);

    auto node = to.node.lock();
    assert(node && node->get_type().is_derived_from<Node>());
    dag::Graph<NodeVarType>::SetTreeDirty(std::static_pointer_cast<Node>(node));

    m_dirty = true;
}

void Evaluator::RebuildConnections(const std::vector<std::pair<dag::Node<NodeVarType>::PortAddr, dag::Node<NodeVarType>::PortAddr>>& conns)
{
    // update dirty
    for (auto itr : m_nodes_map) {
        if (dag::Graph<NodeVarType>::HasNodeConns(itr.second)) {
            dag::Graph<NodeVarType>::SetTreeDirty(itr.second);
        }
    }

    // remove conns
    for (auto itr : m_nodes_map) {
        itr.second->ClearConnections();
    }

    // add conns
    for (auto& conn : conns)
    {
        auto node = conn.second.node.lock();
        assert(node && node->get_type().is_derived_from<Node>());
        dag::Graph<NodeVarType>::SetTreeDirty(std::static_pointer_cast<Node>(node));
        dag::make_connecting<NodeVarType>(conn.first, conn.second);
    }

    m_dirty = true;
}

void Evaluator::Update(const ur::Device& dev, bool force)
{
    if (!force && !m_dirty) {
        return;
    }

    // 1. build node connection for desc calc
    UpdateNodes(dev);
    // 2. calc prop
    UpdateProps();
    // 3. update node finally
    MakeDirty(true);    // todo: set dirty by props
    UpdateNodes(dev);

    m_dirty = false;
}

void Evaluator::MakeDirty(bool all_nodes_dirty)
{
    m_dirty = true;

    if (all_nodes_dirty)
    {
        for (auto itr : m_nodes_map) {
            itr.second->SetDirty(true);
        }
    }
}

dag::Variable Evaluator::CalcExpr(const std::string& str, const EvalContext& ctx) const
{
    if (str.empty()) {
        return dag::Variable();
    }

    cslang::Parser parser(str.c_str());
    auto expr = cslang::ast::ExpressionParser::ParseExpression(parser);
    auto val = cslang::EvalExpression(expr, &ctx);
    switch (val.type)
    {
    case cslang::VarType::Invalid:
        return dag::Variable();
    case cslang::VarType::Bool:
        return dag::Variable(val.b);
    case cslang::VarType::Int:
        return dag::Variable(val.i);
    case cslang::VarType::Float:
        return dag::Variable(val.f);
    case cslang::VarType::Float3:
    {
        auto f3 = static_cast<const float*>(val.p);
        return dag::Variable(sm::vec3(f3[0], f3[1], f3[2]));
    }
    case cslang::VarType::Double:
        return dag::Variable(static_cast<float>(val.d));
    case cslang::VarType::String:
        return dag::Variable(cslang::StringPool::VoidToString(val.p));
    default:
        assert(0);
        return dag::Variable();
    }
}

void Evaluator::RunStatement(const std::string& str, const EvalContext& ctx) const
{
    if (str.empty()) {
        return;
    }

    cslang::Parser parser(str.c_str());
    auto ast = cslang::ast::StatementParser::ParseCompoundStatement(parser);
    cslang::RunStatement(ast, &ctx);
}

void Evaluator::Rename(const std::string& from, const std::string& to)
{
    auto itr_f = m_nodes_map.find(from);
    if (itr_f == m_nodes_map.end()) {
        assert(0);
        return;
    }

    auto node = itr_f->second;
    m_nodes_map.erase(itr_f);

    auto itr_t = m_nodes_map.find(to);
    if (itr_t == m_nodes_map.end())
    {
        node->SetName(to);
    }
    else
    {
        std::string fixed;
        int idx = 0;
        do {
            fixed = to + std::to_string(idx++);
        } while (m_nodes_map.find(fixed) != m_nodes_map.end());

        node->SetName(fixed);
    }

    m_nodes_map.insert({ node->GetName(), node });
}

NodePtr Evaluator::QueryNodeByName(const std::string& name) const
{
    auto itr = m_nodes_map.find(name);
    return itr == m_nodes_map.end() ? nullptr : itr->second;
}

NodePtr Evaluator::QueryNodeByPath(const std::string& path) const
{
    std::vector<std::string> tokens;
    cpputil::StringHelper::Split(path, "/", tokens);
    if (tokens.empty()) {
        return nullptr;
    }

    NodePtr curr_node = QueryNodeByName(tokens.front());
    if (!curr_node) {
        return nullptr;
    }

    int curr_level = curr_node->GetLevel();
    const int begin_level = curr_level;
    for (size_t i = 1, n = tokens.size(); i < n; ++i)
    {
        if (!curr_node && curr_level == begin_level - 1) {
            curr_node = QueryNodeByName(tokens[i]);
            if (curr_node) {
                continue;
            }
        }

        if (!curr_node) {
            return nullptr;
        }

        auto& t = tokens[i];
        if (t == "..") {
            curr_node = curr_node->GetParent();
            --curr_level;
            continue;
        }

        // query child
        assert(curr_node);
        if (curr_node->get_type() == rttr::type::get<node::Subnetwork>())
        {
            auto child = std::static_pointer_cast<node::Subnetwork>(curr_node)->QueryChild(t);
            if (child) {
                curr_node = child;
                continue;
            }
        }
    }
    return curr_node;
}

const Node* Evaluator::QueryNodeByPath(const Node* base, const std::string& path) const
{
    std::vector<std::string> tokens;
    cpputil::StringHelper::Split(path, "/", tokens);
    auto curr_node = base;
    int curr_level = curr_node->GetLevel();
    const int begin_level = curr_level;
    for (size_t i = 0, n = tokens.size(); i < n; ++i)
    {
        if (!curr_node && curr_level == begin_level - 1) {
            curr_node = QueryNodeByName(tokens[i]).get();
            if (curr_node) {
                continue;
            }
        }

        if (!curr_node) {
            return nullptr;
        }

        auto& t = tokens[i];
        if (t == "..") {
            curr_node = curr_node->GetParent().get();
            --curr_level;
            continue;
        }

        // query child
        assert(curr_node);
        if (curr_node->get_type() == rttr::type::get<node::Subnetwork>())
        {
            auto child = static_cast<const node::Subnetwork*>(curr_node)->QueryChild(t);
            if (child) {
                curr_node = child.get();
                continue;
            }
        }
    }
    return curr_node;
}

void Evaluator::UpdateProps()
{
    // todo: topo sort props

    if (m_nodes_sorted.empty()) {
        return;
    }

    bool dirty = false;
    for (auto& node : m_nodes_sorted) {
        if (const_cast<NodeParmsMgr&>(node->GetParms()).Update(*this)) {
            dirty = true;
        }
    }

    // todo: make all dirty
    if (dirty) {
        MakeDirty();
    }
}

void Evaluator::UpdateNodes(const ur::Device& dev)
{
    if (m_nodes_sorted.empty()) {
        return;
    }

    TopologicalSorting();

    std::map<NodePtr, int> map2index;
    for (int i = 0, n = m_nodes_sorted.size(); i < n; ++i) {
        map2index.insert({ m_nodes_sorted[i], i });
    }

    for (int i = 0, n = m_nodes_sorted.size(); i < n; ++i)
    {
        auto& node = m_nodes_sorted[i];
        if (node->IsDirty()) {
            node->Execute(dev, *this);
            node->SetDirty(false);
        }
    }
}

void Evaluator::TopologicalSorting()
{
    std::vector<NodePtr> nodes;
    nodes.reserve(m_nodes_map.size());
    for (auto itr : m_nodes_map) {
        nodes.push_back(itr.second);
    }

    // prepare
    std::vector<int> in_deg(m_nodes_map.size(), 0);
    std::vector<std::vector<int>> out_nodes(nodes.size());
    for (int i = 0, n = nodes.size(); i < n; ++i)
    {
        auto& node = nodes[i];
        if (node->get_type() == rttr::type::get<node::ObjectMerge>())
        {
            for (auto& path : std::static_pointer_cast<node::ObjectMerge>(node)->GetObjectPaths())
            {
                auto cnode = QueryNodeByPath(node.get(), path);
                if (!cnode) {
                    continue;
                }
                for (int j = 0, m = nodes.size(); j < m; ++j) {
                    if (cnode == nodes[j].get()) {
                        in_deg[i]++;
                        out_nodes[j].push_back(i);
                        break;
                    }
                }
            }
        }
        else
        {
            for (auto& port : node->GetImports())
            {
                if (port.conns.empty()) {
                    continue;
                }

                assert(port.conns.size() == 1);
                auto from = port.conns[0].node.lock();
                assert(from);
                for (int j = 0, m = nodes.size(); j < m; ++j) {
                    if (from == nodes[j]) {
                        in_deg[i]++;
                        out_nodes[j].push_back(i);
                        break;
                    }
                }
            }
        }
    }

    // sort
    std::stack<int> st;
    m_nodes_sorted.clear();
    for (int i = 0, n = in_deg.size(); i < n; ++i) {
        if (in_deg[i] == 0) {
            st.push(i);
        }
    }
    while (!st.empty())
    {
        int v = st.top();
        st.pop();
        m_nodes_sorted.push_back(nodes[v]);
        for (auto& i : out_nodes[v]) {
            assert(in_deg[i] > 0);
            in_deg[i]--;
            if (in_deg[i] == 0) {
                st.push(i);
            }
        }
    }
}

}