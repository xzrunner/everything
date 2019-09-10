#pragma once

#include "everything/Node.h"
#include "everything/Group.h"

namespace evt
{
namespace node
{

class GroupExpression : public Node
{
public:
    struct Instance
    {
        std::string group_name;
        std::string expr_str;
        GroupMerge  merge_op = GroupMerge::Union;
    };

public:
    GroupExpression()
    {
        m_imports = {
            {{ NodeVarType::Any, "in0" }},
            {{ NodeVarType::Any, "in1" }},
            {{ NodeVarType::Any, "in2" }},
            {{ NodeVarType::Any, "in3" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval, TreeContext& ctx) override;

    void SetGroupType(GroupType type);

    void ClearInstances();
    void AddInstance(const Instance& inst);

private:
    void Select(std::vector<size_t>& items,
        const Instance& inst, Evaluator& eval) const;
    void Merge(MergeOP op, const std::vector<size_t>& src,
        std::vector<size_t>& dst) const;

private:
    GroupType m_group_type = GroupType::Primitives;

    std::vector<Instance> m_intsts;

    RTTR_ENABLE(Node)

}; // GroupExpression

}
}