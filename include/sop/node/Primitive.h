#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"

#include <SM_Vector.h>
#include <SM_Matrix.h>

namespace sop
{
namespace node
{

class Primitive : public Node
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
    Primitive()
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

    void SetTranslate(const sm::vec3& t);
    void SetRotate(const sm::vec3& r);
    void SetScale(const sm::vec3& s);
    void SetShear(const sm::vec3& s);

private:
    void InitProps();

    void UpdatePrim(GeoAttribute::Primitive& prim, const sm::mat4& mat);

    sm::mat4 CalcTransformMat() const;

private:
    std::string m_group_name;

    RTTR_ENABLE(Node)

}; // Primitive

}
}