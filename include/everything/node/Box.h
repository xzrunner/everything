#pragma once

#include "everything/Node.h"

#include <SM_Vector.h>

namespace n3 { class CompTransform; }
namespace model { class BrushModel; }

namespace evt
{
namespace node
{

class Box : public Node
{
public:
    Box()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    void SetSize(const sm::vec3& size);
    void SetCenter(const sm::vec3& center);
    void SetScale(const sm::vec3& scale);

private:
    void BuildModel();

    std::unique_ptr<model::BrushModel> BuildBrush() const;

private:
    sm::vec3 m_size   = sm::vec3(1, 1, 1);
    sm::vec3 m_center = sm::vec3(0, 0, 0);
    sm::vec3 m_scale  = sm::vec3(1, 1, 1);

    RTTR_ENABLE(Node)

}; // Box

}
}