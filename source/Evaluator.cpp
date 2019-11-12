#include "sop/Evaluator.h"
#include "sop/EvalContext.h"
#include "sop/node/Geometry.h"
#include "sop/node/ObjectMerge.h"

#include <vexc/EvalAST.h>
#include <vexc/Parser.h>
#include <cpputil/StringHelper.h>

#include <stack>
#include <queue>

#include <assert.h>

namespace sop
{

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

    SetTreeDirty(node);

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
    SetTreeDirty(node);

    m_dirty = true;
}

void Evaluator::Connect(const Node::PortAddr& from, const Node::PortAddr& to)
{
    make_connecting(from, to);

    auto node = to.node.lock();
    assert(node);
    SetTreeDirty(node);

    m_dirty = true;
}

void Evaluator::Disconnect(const Node::PortAddr& from, const Node::PortAddr& to)
{
    disconnect(from, to);

    auto node = to.node.lock();
    assert(node);
    SetTreeDirty(node);

    m_dirty = true;
}

void Evaluator::RebuildConnections(const std::vector<std::pair<Node::PortAddr, Node::PortAddr>>& conns)
{
    // update dirty
    for (auto itr : m_nodes_map) {
        if (HasNodeConns(itr.second)) {
            SetTreeDirty(itr.second);
        }
    }

    // remove conns
    for (auto itr : m_nodes_map)
    {
        for (auto& in : itr.second->GetImports()) {
            const_cast<Node::Port&>(in).conns.clear();
        }
        for (auto& out : itr.second->GetExports()) {
            const_cast<Node::Port&>(out).conns.clear();
        }
    }

    // add conns
    for (auto& conn : conns)
    {
        auto t_node = conn.second.node.lock();
        assert(t_node);
        SetTreeDirty(t_node);
        make_connecting(conn.first, conn.second);
    }

    m_dirty = true;
}

void Evaluator::Update()
{
    if (!m_dirty) {
        return;
    }

    // 1. build node connection for desc calc
    UpdateNodes();
    // 2. calc prop
    UpdateProps();
    // 3. update node finally
    MakeDirty(true);    // todo: set dirty by props
    UpdateNodes();

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

Variable Evaluator::CalcExpr(const std::string& str, const EvalContext& ctx) const
{
    if (str.empty()) {
        return Variable();
    }

    vexc::Parser parser(str.c_str());
    auto expr = vexc::ast::ExpressionParser::ParseExpression(parser);
    auto val = vexc::EvalExpression(expr, &ctx);
    switch (val.type)
    {
    case vexc::VarType::Invalid:
        return Variable();
    case vexc::VarType::Bool:
        return Variable(val.b);
    case vexc::VarType::Int:
        return Variable(val.i);
    case vexc::VarType::Float:
        return Variable(val.f);
    case vexc::VarType::Float3:
    {
        auto f3 = static_cast<const float*>(val.p);
        return Variable(sm::vec3(f3[0], f3[1], f3[2]));
    }
    case vexc::VarType::Double:
        return Variable(static_cast<float>(val.d));
    case vexc::VarType::String:
        return Variable(vexc::StringPool::VoidToString(val.p));
    default:
        assert(0);
        return Variable();
    }
}

void Evaluator::RunStatement(const std::string& str, const EvalContext& ctx) const
{
    if (str.empty()) {
        return;
    }

    vexc::Parser parser(str.c_str());
    auto ast = vexc::ast::StatementParser::ParseCompoundStatement(parser);
    vexc::RunStatement(ast, &ctx);
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
        if (curr_node->get_type() == rttr::type::get<node::Geometry>())
        {
            auto child = std::static_pointer_cast<node::Geometry>(curr_node)->QueryChild(t);
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
        if (curr_node->get_type() == rttr::type::get<node::Geometry>())
        {
            auto child = static_cast<const node::Geometry*>(curr_node)->QueryChild(t);
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
        for (auto& prop : node->GetProps().GetProps()) {
            if (const_cast<NodeProp&>(prop).Update(*this, node)) {
                dirty = true;
            }
        }
    }

    // todo: make all dirty
    if (dirty) {
        MakeDirty();
    }
}

void Evaluator::UpdateNodes()
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
            node->Execute(*this);
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
            for (auto& obj : std::static_pointer_cast<node::ObjectMerge>(node)->GetObjects()) {
                for (int j = 0, m = nodes.size(); j < m; ++j) {
                    if (obj == nodes[j]) {
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

void Evaluator::SetTreeDirty(const NodePtr& root)
{
    std::queue<const Node*> buf;
    buf.push(root.get());
    while (!buf.empty())
    {
        auto n = buf.front(); buf.pop();
        n->SetDirty(true);
        for (auto& port : n->GetExports()) {
            for (auto& conn : port.conns) {
                auto node = conn.node.lock();
                buf.push(node.get());
            }
        }
    }
}

bool Evaluator::HasNodeConns(const NodePtr& node)
{
    for (auto& i : node->GetImports()) {
        if (!i.conns.empty()) {
            return true;
        }
    }
    for (auto& o : node->GetExports()) {
        if (!o.conns.empty()) {
            return true;
        }
    }
    return false;
}

}