#pragma once

#include "everything/Node.h"

#include <SM_Vector.h>

namespace evt
{
namespace node
{

class Curve : public Node
{
public:
    Curve()
    {
        m_imports = {
            {{ VariableType::Any, "in" }}
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    auto& GetVertices() const { return m_vertices; }
    void  SetVertices(const std::vector<sm::vec3>& vertices);

private:
    void BuildModel();

private:
    std::vector<sm::vec3> m_vertices;

    RTTR_ENABLE(Node)

}; // Curve

}
}