#pragma once

#include "sop/Node.h"

#include <SM_Vector.h>

namespace sop
{
namespace node
{

class Line : public Node
{
public:
    enum PropID
    {
        LENGTH,

        MAX_BUILD_IN_PROP,
    };

    static const constexpr char* const PropNames[MAX_BUILD_IN_PROP] = {
        "dist"
    };

public:
    Line()
        : Node(MAX_BUILD_IN_PROP)
    {
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };

        InitProps();
    }

    virtual void Execute(Evaluator& eval) override;

    void SetOrigin(const sm::vec3& ori);
    void SetDirection(const sm::vec3& dir);
    void SetLength(float len);
    void SetPoints(size_t num);

private:
    void InitProps();

    void BuildModel();

private:
    sm::vec3 m_origin    = sm::vec3(0, 0, 0);
    sm::vec3 m_direction = sm::vec3(0, 1, 0);

    size_t m_points = 2;

    RTTR_ENABLE(Node)

}; // Line

}
}