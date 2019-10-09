#pragma once

#include "sop/Node.h"
#include "sop/GeoAttrClass.h"

#include <SM_Vector.h>

namespace sop
{
namespace node
{

class Color : public Node
{
public:
    Color()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetAttrAddTo(GeoAttrClass attr);

    void SetColor(const sm::vec3& col);

private:
    GeoAttrClass m_attr_add_to = GeoAttrClass::Point;

    sm::vec3 m_color = sm::vec3(1.0f, 1.0f, 1.0f);

    RTTR_ENABLE(Node)

}; // Color

}
}