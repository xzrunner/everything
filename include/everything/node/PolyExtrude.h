#pragma once

#include "everything/Node.h"

#include <model/BrushModel.h>

namespace evt
{
namespace node
{

class PolyExtrude : public Node
{
public:
    PolyExtrude()
    {
        m_imports = {
            {{ VariableType::Any, "in" }}
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void ExecuteSelf() override;

    void SetGroup(const std::shared_ptr<model::BrushModel::BrushGroup>& group);

    void SetDistance(float dist);

private:
    std::shared_ptr<model::BrushModel::BrushGroup> m_group = nullptr;

    float m_distance = 0;

    RTTR_ENABLE(Node)

}; // PolyExtrude

}
}