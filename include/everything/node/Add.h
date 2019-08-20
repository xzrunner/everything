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
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    void SetPoints(const std::vector<sm::vec3>& points);

private:
    std::vector<sm::vec3> m_points;

    RTTR_ENABLE(Node)

}; // Add

}
}