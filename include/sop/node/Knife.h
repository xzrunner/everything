#pragma once

#include "sop/Node.h"

#include <SM_Vector.h>

namespace pm3 { class Polytope; }

namespace sop
{
namespace node
{

class Knife : public Node
{
public:
    enum PropID
    {
        ORIGINX_X,
        ORIGINX_Y,
        ORIGINX_Z,

        MAX_BUILD_IN_PROP,
    };

    static const constexpr char* const PropNames[MAX_BUILD_IN_PROP] = {
        "originx", "originy", "originz",
    };

public:
    Knife()
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

    void SetOrigin(const sm::vec3& origin);
    void SetDirection(const sm::vec3& dir);

public:
    enum class KeepType
    {
        KeepAbove,
        KeepBelow,
        KeepAll,
    };

    void SetKeepType(KeepType keep);

private:
    void InitProps();

    bool Clip(pm3::Polytope& poly) const;

private:
    sm::vec3 m_direction = sm::vec3(0, 1, 0);

    KeepType m_keep = KeepType::KeepAll;

    RTTR_ENABLE(Node)

}; // Knife

}
}