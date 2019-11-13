#pragma once

#include "sop/Node.h"

#include <SM_Vector.h>

namespace model { class BrushModel; }

namespace sop
{
namespace node
{

class Box : public Node
{
public:
    enum PropID
    {
        SIZE,
        POS,

        MAX_BUILD_IN_PROP,
    };

    static const constexpr char* const PropNames[MAX_BUILD_IN_PROP] = {
        "size", "t"
    };

public:
    Box()
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

    void SetSize(const sm::vec3& size);
    void SetCenter(const sm::vec3& center);
    void SetScale(const sm::vec3& scale);

private:
    void InitProps();

    void BuildModel();

    std::unique_ptr<model::BrushModel> BuildBrush() const;

private:
    sm::vec3 m_scale  = sm::vec3(1, 1, 1);

    RTTR_ENABLE(Node)

}; // Box

}
}