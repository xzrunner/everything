#include "everything/NodePropsMgr.h"
#include "everything/Evaluator.h"
#include "everything/EvalContext.h"

namespace evt
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
    for (auto& p : m_props) {
        if (p.m_key == key) {
            return p.m_val;
        }
    }
    return Variable();
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
    if (m_expr.empty()) {
        return false;
    }

    auto val = eval.CalcExpr(m_expr, EvalContext(eval, *node));
    if (val.type == VarType::Invalid) {
        return false;
    }

    if (val.type != m_val.type)
    {
        switch (m_val.type)
        {
        case VarType::Int:
        {
            switch (val.type)
            {
            case VarType::Float:
            {
                auto tmp = static_cast<int>(val.f);
                val.i = tmp;
            }
                break;
            default:
                assert(0);
                return false;
            }
        }
            break;
        case VarType::Float:
        {
            switch (val.type)
            {
            case VarType::Int:
            {
                auto tmp = static_cast<float>(val.i);
                val.f = tmp;
            }
                break;
            default:
                assert(0);
                return false;
            }
        }
            break;
        default:
            assert(0);
            return false;
        }
        val.type = m_val.type;
    }

    assert(val.type == m_val.type);
    if (m_val == val) {
        return false;
    } else {
        m_val = val;
        return true;
    }
}

}