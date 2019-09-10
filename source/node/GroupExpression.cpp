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
            GroupMgr::Merge(inst.merge_op, items, group->items);
        }
        else
        {
            group = std::make_shared<Group>();
            group->name  = inst.group_name;
            group->type  = m_group_type;
            group->items = items;
            group_mgr.Add(group, inst.merge_op);
            inst.group_name = group->name;
        }
    }
}

void GroupExpression::SetGroupType(GroupType type)
{
    if (m_group_type == type) {
        return;
    }

    m_group_type = type;

    SetDirty(true);
}

void GroupExpression::ClearInstances()
{
    if (m_intsts.empty()) {
        return;
    }

    m_intsts.clear();
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
    switch (m_group_type)
    {
    case GroupType::Points:
    {
        EvalContext eval_ctx(eval, *this);
        auto& pts = m_geo_impl->GetAttr().GetPoints();
        for (size_t i = 0, n = pts.size(); i < n; ++i)
        {
            eval_ctx.point_idx = i;
            auto v = eval.CalcExpr(inst.expr_str, eval_ctx);
            if (v.type == VariableType::Invalid) {
                continue;
            }
            assert(v.type == VariableType::Bool);
            if (v.b) {
                items.push_back(i);
            }
        }
    }
    break;
    }
}

}
}