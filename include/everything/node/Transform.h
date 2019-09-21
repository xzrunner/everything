#pragma once

#include "everything/Node.h"
#include "everything/Group.h"

#include <SM_Vector.h>
#include <SM_Matrix.h>

namespace evt
{
namespace node
{

class Transform : public Node
{
public:
    enum PropID
    {
        TRANS_X,
        TRANS_Y,
        TRANS_Z,

        ROT_X,
        ROT_Y,
        ROT_Z,

        SCALE_X,
        SCALE_Y,
        SCALE_Z,

        SHEAR_X,
        SHEAR_Y,
        SHEAR_Z,

        MAX_BUILD_IN_PROP,
    };

    static const constexpr char* const PropNames[MAX_BUILD_IN_PROP] = {
        "tx", "ty", "tz",
        "rx", "ry", "rz",
        "sx", "sy", "sz",
        "shear1", "shear2", "shear3"
    };

public:
    Transform()
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

    void SetGroupName(const std::string& name) { m_group_name = name; }
    void SetGroupType(GroupType type);

private:
    void InitProps();

    sm::mat4 CalcTransformMat() const;

private:
    std::string m_group_name;
    GroupType   m_group_type = GroupType::GuessFromGroup;

    RTTR_ENABLE(Node)

}; // Transform

}
}