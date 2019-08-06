#pragma once

#include "everything/Node.h"

#include <SM_Vector.h>

namespace evt
{
namespace node
{

class Transform : public Node
{
public:
    Transform()
    {
        m_imports = {
            {{ VariableType::Any, "in" }}
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    void SetTranslate(const sm::vec3& t);
    void SetRotate(const sm::vec3& r);
    void SetScale(const sm::vec3& s);
    void SetShear(const sm::vec3& s);

private:
    sm::vec3 m_translate = sm::vec3(0, 0, 0);
    sm::vec3 m_rotate    = sm::vec3(0, 0, 0);
    sm::vec3 m_scale     = sm::vec3(1, 1, 1);
    sm::vec3 m_shear     = sm::vec3(0, 0, 0);

    RTTR_ENABLE(Node)

}; // Transform

}
}