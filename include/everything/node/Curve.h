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
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

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