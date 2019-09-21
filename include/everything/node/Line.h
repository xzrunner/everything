#pragma once

#include "everything/Node.h"

#include <SM_Vector.h>

namespace evt
{
namespace node
{

class Line : public Node
{
public:
    Line()
    {
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetOrigin(const sm::vec3& ori);
    void SetDirection(const sm::vec3& dir);
    void SetLength(float len);
    void SetPoints(size_t num);

private:
    void BuildModel();

private:
    sm::vec3 m_origin    = sm::vec3(0, 0, 0);
    sm::vec3 m_direction = sm::vec3(0, 0, 1);

    float m_length = 1;

    size_t m_points = 2;

    RTTR_ENABLE(Node)

}; // Line

}
}