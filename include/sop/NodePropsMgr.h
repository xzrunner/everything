#pragma once

#include "sop/Variable.h"

#include <string>
#include <vector>
#include <map>

namespace sop
{

class Node;
class Evaluator;
struct EvalContext;

class NodeProp
{
public:
    NodeProp() {}
    NodeProp(const std::string& key, const Variable& val)
        : m_key(key), m_val(val) {}

    auto& Val() const { return m_val; }

    void Clear();

    bool Update(const Evaluator& eval, const std::shared_ptr<Node>& node);

private:
    static Variable CalcExpr(const Evaluator& eval, const std::shared_ptr<Node>& node,
        const std::string& expr, const EvalContext& ctx, VarType exp_type);

private:
    std::string m_key;
    Variable    m_val;

    std::string m_expr[3];

//    std::vector<std::weak_ptr<NodeProp>> m_from, m_to;

    friend class NodePropsMgr;

}; // NodeProp

class NodePropsMgr
{
public:
    NodePropsMgr(size_t build_in_count);

    void Assign(size_t idx, const std::string& key, const Variable& val);

    void SetExpr(size_t idx, const std::string& expr, size_t comp_idx = 0);
    bool SetValue(size_t idx, const Variable& val, bool float_cast = false);
    bool SetValue(const std::string& key, const Variable& val);

    int  Add(const std::string& key, const Variable& val);
    bool Remove(const std::string& key);
    void Clear();

    Variable Query(const std::string& key) const;

    size_t Size() const { return m_props.size(); }

    auto& GetProps() const { return m_props; }

    int QueryIndex(const std::string& key) const;

private:
    size_t m_build_in_count = 0;

    std::vector<NodeProp> m_props;

}; // NodePropsMgr

}