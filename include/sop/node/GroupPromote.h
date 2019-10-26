#pragma once

#include "sop/Node.h"
#include "sop/Group.h"

namespace sop
{
namespace node
{

class GroupPromote : public Node
{
public:

public:
    GroupPromote()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetGroupName(const std::string& name);

    void SetSrcGroupType(GroupType type);
    void SetDstGroupType(GroupType type);

private:
    void PrimsToPoints(Group& group);
    void PointsToPrims(Group& group);

private:
    std::string m_group_name;

    GroupType m_src_type = GroupType::GuessFromGroup;
    GroupType m_dst_type = GroupType::Points;

    RTTR_ENABLE(Node)

}; // GroupPromote

}
}