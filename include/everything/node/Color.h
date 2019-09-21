#pragma once

#include "everything/Node.h"
#include "everything/GeoAttrType.h"

#include <SM_Vector.h>

namespace evt
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

    void SetAttrAddTo(GeoAttrType attr);

    void SetColor(const sm::vec3& col);

private:
    GeoAttrType m_attr_add_to = GeoAttrType::Point;

    sm::vec3 m_color = sm::vec3(1.0f, 1.0f, 1.0f);

    RTTR_ENABLE(Node)

}; // Color

}
}