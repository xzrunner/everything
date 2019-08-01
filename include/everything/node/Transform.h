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

    void SetTranslate(const sm::vec3& t) { m_translate = t; }
    void SetRotate(const sm::vec3& r) { m_rotate = r; }
    void SetScale(const sm::vec3& s) { m_scale = s; }
    void SetShear(const sm::vec3& s) { m_shear = s; }

protected:
    virtual void ExecuteSelf() override;

private:
    sm::vec3 m_translate = sm::vec3(0, 0, 0);
    sm::vec3 m_rotate    = sm::vec3(0, 0, 0);
    sm::vec3 m_scale     = sm::vec3(1, 1, 1);
    sm::vec3 m_shear     = sm::vec3(0, 0, 0);

    RTTR_ENABLE(Node)

}; // Transform

}
}