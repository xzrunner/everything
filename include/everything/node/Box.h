#pragma once

#include "everything/Node.h"

#include <geoshape/Box.h>
#include <node0/typedef.h>

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
            {{ VariableType::Port, "prev" }}
        };
        m_exports = {
            {{ VariableType::Port,      "next" }},
//            {{ VariableType::Primitive, "prim" }}
        };
    }

    virtual void Execute() override;
    virtual void Traverse(std::function<bool(const n0::SceneNodePtr&)> func) const override;

    void SetSize(const sm::vec3& size);
    void SetCenter(const sm::vec3& center);
    void SetScale(const sm::vec3& scale);

private:
    void UpdateModel();

    std::unique_ptr<model::BrushModel> BuildBrush() const;

private:
    gs::Box m_box;

    n0::SceneNodePtr m_node = nullptr;

    sm::vec3 m_size   = sm::vec3(1, 1, 1);
    sm::vec3 m_center = sm::vec3(0, 0, 0);
    sm::vec3 m_scale  = sm::vec3(1, 1, 1);

    RTTR_ENABLE(Node)

}; // Box

}
}