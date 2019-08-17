#pragma once

#include "everything/Node.h"

#include <SM_Vector.h>

namespace evt
{
namespace node
{

class Add : public Node
{
public:
    Add()
    {
        m_imports = {
            {{ VariableType::Any, "in" }}
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    void SetPoints(const std::vector<sm::vec3>& points);

private:
    void BuildModel(const std::vector<sm::vec3>& vertices);

private:
    std::vector<sm::vec3> m_points;

    RTTR_ENABLE(Node)

}; // Add

}
}