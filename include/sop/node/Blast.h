#pragma once

#include "sop/Node.h"
#include "sop/Group.h"
#include "sop/GeoAttrClass.h"

namespace sop
{
namespace node
{

class Blast : public Node
{
public:
    enum PropID
    {
        GROUP,
        DEL_NON_SEL,

        MAX_BUILD_IN_PROP,
    };

    static const constexpr char* const PropNames[MAX_BUILD_IN_PROP] = {
        "group", "negate"
    };

public:
    Blast()
        : Node(MAX_BUILD_IN_PROP)
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };

        InitProps();
    }

    virtual void Execute(Evaluator& eval) override;

    void SetGroupName(const std::string& name);
    void SetGroupType(GroupType type);

    void SetDeleteNonSelected(bool del_non_selected);

private:
    void InitProps();

    bool SetupDelFlags(const Group& group, size_t count,
        std::vector<bool>& del_flags) const;

private:
    GroupType m_group_type = GroupType::GuessFromGroup;

    RTTR_ENABLE(Node)

}; // Blast

}
}