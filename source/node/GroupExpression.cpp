#include "sop/node/GroupExpression.h"
#include "sop/NodeHelper.h"
#include "sop/Group.h"
#include "sop/EvalContext.h"
#include "sop/Evaluator.h"
#include "sop/GeometryImpl.h"

namespace sop
{
namespace node
{

void GroupExpression::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    for (auto& inst : m_insts)
    {
        auto items = NodeHelper::SelectGeoByExpr(m_group_type, eval, *this, inst.expr_str);
        auto& group_mgr = m_geo_impl->GetGroup();
        auto group = group_mgr.Query(inst.group_name);
        if (group)
        {
            Group::Merge(inst.merge_op, items, const_cast<std::vector<size_t>&>(group->GetItems()));
        }
        else
        {
            group = std::make_shared<Group>();
            group->SetName(inst.group_name);
            group->SetType(m_group_type);
            group->SetItems(items);
            group_mgr.Add(group, inst.merge_op);
            inst.group_name = group->GetName();
        }
    }
}

void GroupExpression::SetGroupType(GroupType type)
{
    NODE_PROP_SET(m_group_type, type);
}

void GroupExpression::ClearInstances()
{
    if (m_insts.empty()) {
        return;
    }

    m_insts.clear();
    SetDirty(true);
}

void GroupExpression::AddInstance(const Instance& inst)
{
    m_insts.push_back(inst);
    SetDirty(true);
}

}
}