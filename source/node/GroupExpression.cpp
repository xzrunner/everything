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

void GroupExpression::Execute(const ur2::Device& dev, Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    for (size_t i = 0, n = GetExprsNum(); i < n; ++i)
    {
        auto items = NodeHelper::SelectGeoByExpr(m_group_type, eval, *this, m_group_exprs[i]);
        auto& group_mgr = m_geo_impl->GetGroup();
        auto group = group_mgr.Query(m_group_names[i]);
        if (group)
        {
            Group::Merge(m_group_merge_ops[i], items, const_cast<std::vector<size_t>&>(group->GetItems()));
        }
        else
        {
            group = std::make_shared<Group>();
            group->SetName(m_group_names[i]);
            group->SetType(m_group_type);
            group->SetItems(items);
            group_mgr.Add(group, m_group_merge_ops[i]);
            m_group_names[i] = group->GetName();
        }
    }
}

void GroupExpression::SetExprsNum(size_t num)
{
    m_group_names.resize(num);
    m_group_exprs.resize(num);
    m_group_merge_ops.resize(num, GroupMerge::Union);
}

size_t GroupExpression::GetExprsNum() const
{
    assert(m_group_names.size() == m_group_exprs.size()
        && m_group_exprs.size() == m_group_merge_ops.size());
    return m_group_names.size();
}

}
}