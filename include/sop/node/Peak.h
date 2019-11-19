#pragma once

#include "sop/Node.h"
#include "sop/Group.h"

namespace sop
{
namespace node
{

class Peak : public Node
{
public:
    enum PropID
    {
        DIST,

        MAX_BUILD_IN_PROP,
    };

    static const constexpr char* const PropNames[MAX_BUILD_IN_PROP] = {
        "dist"
    };

public:
    Peak()
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

    void SetDistance(float dist);

    void SetUpdateNorm(bool update_norm);

private:
    void InitProps();

    void TranslatePoints(float dist, const std::shared_ptr<Group>& group);
    void TranslateVertices(float dist, const std::shared_ptr<Group>& group);
    void TranslatePrimitives(float dist, const std::shared_ptr<Group>& group);

private:
    std::string m_group_name;
    GroupType   m_group_type = GroupType::GuessFromGroup;

    // recompute point normals
    bool m_update_normals = true;

    RTTR_ENABLE(Node)

}; // Peak

}
}