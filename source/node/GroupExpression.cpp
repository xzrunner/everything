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