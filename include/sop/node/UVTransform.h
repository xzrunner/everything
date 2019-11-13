#pragma once

#include "sop/Node.h"
#include "sop/Group.h"

#include <SM_Vector.h>
#include <SM_Matrix.h>

namespace sop
{
namespace node
{

class UVTransform : public Node
{
public:
    enum PropID
    {
        TRANS,
        ROT,
        SCALE,
        SHEAR,

        MAX_BUILD_IN_PROP,
    };

    static const constexpr char* const PropNames[MAX_BUILD_IN_PROP] = {
        "t", "r", "s", "shear"
    };

public:
    UVTransform()
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

    void SetTranslate(const sm::vec3& t);
    void SetRotate(const sm::vec3& r);
    void SetScale(const sm::vec3& s);
    void SetShear(const sm::vec3& s);

    void SetGroupName(const std::string& name);
    void SetGroupType(GroupType type);

private:
    void InitProps();

    sm::mat4 CalcTransformMat() const;

private:
    std::string m_group_name;
    GroupType   m_group_type = GroupType::GuessFromGroup;

    RTTR_ENABLE(Node)

}; // UVTransform

}
}