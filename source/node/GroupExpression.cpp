#include "everything/node/GroupExpression.h"
#include "everything/NodeHelper.h"
#include "everything/Group.h"
#include "everything/EvalContext.h"
#include "everything/Evaluator.h"
#include "everything/GeometryImpl.h"

namespace evt
{
namespace node
{

void GroupExpression::Execute(Evaluator& eval, TreeContext& ctx)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    for (auto& inst : m_intsts)
    {
        std::vector<size_t> items;
        Select(items, inst, eval);

        auto& group_mgr = m_geo_impl->GetGroup();
        auto group = group_mgr.Query(inst.group_name);
        if (group)
        {
            Merge(inst.merge_op, items, group->items);
        }
        else
        {
            group = std::make_shared<Group>();
            group->name  = inst.group_name;
            group->type  = m_type;
            group->items = items;
            group_mgr.Add(group);
        }
    }
}

void GroupExpression::SetType(GroupType type)
{
    if (m_type == type) {
        return;
    }

    m_type = type;

    SetDirty(true);
}

void GroupExpression::AddInstance(const Instance& inst)
{
    m_intsts.push_back(inst);
    SetDirty(true);
}

void GroupExpression::Select(std::vector<size_t>& items,
                             const Instance& inst, Evaluator& eval) const
{
    switch (m_type)
    {
    case GroupType::Points:
    {
        EvalContext eval_ctx(eval, *this);
        auto& pts = m_geo_impl->GetAttr().GetPoints();
        for (size_t i = 0, n = pts.size(); i < n; ++i)
        {
            eval_ctx.point_idx = i;
            auto v = eval.CalcExpr(inst.expr_str, eval_ctx);
            assert(v.type == VariableType::Bool);
            if (v.b) {
                items.push_back(i);
            }
        }
    }
    break;
    }
}

void GroupExpression::Merge(MergeOP op, const std::vector<size_t>& src,
                            std::vector<size_t>& dst) const
{
    switch (op)
    {
    case MergeOP::Replace:
        dst = src;
        break;
    case MergeOP::Union:
    {
        std::set<size_t> dst_set;
        for (auto& d : dst) {
            dst_set.insert(d);
        }
        for (auto& s : src) {
            if (dst_set.find(s) == dst_set.end()) {
                dst.push_back(s);
            }
        }
    }
        break;
    case MergeOP::Intersect:
    {
        std::set<size_t> dst_set;
        for (auto& d : dst) {
            dst_set.insert(d);
        }
        dst.clear();
        for (auto& s : src) {
            if (dst_set.find(s) != dst_set.end()) {
                dst.push_back(s);
            }
        }
    }
        break;
    case MergeOP::Subtract:
    {
        for (auto& s : src)
        {
            for (auto itr = dst.begin(); itr != dst.end(); )
            {
                if (s == *itr) {
                    itr = dst.erase(itr);
                } else {
                    ++itr;
                }
            }
        }
    }
        break;
    }
}

}
}