#include "sop/NodePropsMgr.h"
#include "sop/Evaluator.h"
#include "sop/EvalContext.h"

#include <vexc/StringPool.h>

namespace sop
{

NodePropsMgr::NodePropsMgr(size_t build_in_count)
    : m_build_in_count(build_in_count)
{
    m_props.resize(m_build_in_count);
}

void NodePropsMgr::Assign(size_t idx, const std::string& key, const Variable& val)
{
    if (idx < 0 || idx >= m_props.size()) {
        return;
    }

    auto& p = m_props[idx];
    p.Clear();

    p.m_key = key;
    p.m_val = val;
}

void NodePropsMgr::SetExpr(size_t idx, const std::string& expr)
{
    if (idx < 0 || idx >= m_props.size()) {
        return;
    }

    auto& p = m_props[idx];
    if (p.m_expr == expr) {
        return;
    }
    p.m_expr = expr;
}

bool NodePropsMgr::SetValue(size_t idx, const Variable& val)
{
    if (idx < 0 || idx >= m_props.size()) {
        return false;
    }

    auto& p = m_props[idx];
    assert(p.m_val.type == val.type);
    if (p.m_val == val) {
        return false;
    }

    p.m_val = val;

    return true;
}

bool NodePropsMgr::SetValue(const std::string& key, const Variable& val)
{
    auto idx = QueryIndex(key);
    return idx < 0 ? false : SetValue(idx, val);
}

int NodePropsMgr::Add(const std::string& key, const Variable& val)
{
    for (auto& p : m_props) {
        if (p.m_key == key) {
            return -1;
        }
    }

    m_props.push_back({ key, val });

    return m_props.size() - 1;
}

bool NodePropsMgr::Remove(const std::string& key)
{
    for (size_t i = 0, n = m_props.size(); i < n; ++i)
    {
        if (m_props[i].m_key != key) {
            continue;
        }

        if (i < m_build_in_count) {
            return false;
        } else {
            m_props.erase(m_props.begin() + i);
            return true;
        }
    }
    return false;
}

void NodePropsMgr::Clear()
{
    m_props.erase(m_props.begin() + m_build_in_count, m_props.end());
}

Variable NodePropsMgr::Query(const std::string& key) const
{
    auto idx = QueryIndex(key);
    return idx < 0 ? Variable() : m_props[idx].m_val;
}

int NodePropsMgr::QueryIndex(const std::string& key) const
{
    for (size_t i = 0, n = m_props.size(); i < n; ++i) {
        if (m_props[i].m_key == key) {
            return i;
        }
    }
    return -1;
}

//////////////////////////////////////////////////////////////////////////
// class NodeProp
//////////////////////////////////////////////////////////////////////////

void NodeProp::Clear()
{
    m_key.clear();
    m_val.type = VarType::Invalid;

    m_expr.clear();

    m_from.clear();
    m_to.clear();
}

bool NodeProp::Update(const Evaluator& eval, const std::shared_ptr<Node>& node)
{
    auto var = CalcExpr(eval, node, m_expr, EvalContext(eval, *node), m_val.type);
    if (var.type == VarType::Invalid) {
        return false;
    } else {
        assert(var.type == m_val.type);
        if (m_val == var) {
            return false;
        } else {
            m_val = var;
            return true;
        }
    }
}

Variable NodeProp::CalcExpr(const Evaluator& eval, const std::shared_ptr<Node>& node,
                            const std::string& expr, const EvalContext& ctx, VarType exp_type)
{
    if (expr.empty()) {
        return Variable();
    }

    auto val = eval.CalcExpr(expr, EvalContext(eval, *node));
    if (val.type == VarType::Invalid) {
        return Variable();
    }
    if (val.type == exp_type) {
        return val;
    }

    // type conv
    switch (exp_type)
    {
    case sop::VarType::Int:
    {
        switch (val.type)
        {
        case sop::VarType::Float:
        {
            auto tmp = static_cast<int>(val.f);
            val.i = tmp;
        }
            break;
        default:
            assert(0);
            return Variable();
        }
    }
        break;
    case sop::VarType::Float:
    {
        switch (val.type)
        {
        case sop::VarType::Int:
        {
            auto tmp = static_cast<float>(val.i);
            val.f = tmp;
        }
            break;
        case sop::VarType::String:
        {
            auto str = vexc::StringPool::VoidToString(val.p);
            return CalcExpr(eval, node, str, ctx, exp_type);
        }
            break;
        default:
            assert(0);
            return Variable();
        }
    }
        break;
    default:
        assert(0);
        return Variable();
    }

    val.type = exp_type;
    return val;
}

}