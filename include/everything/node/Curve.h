#pragma once

#include "everything/Node.h"

#include <geoshape/Polyline.h>

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
            {{ VariableType::Any, "in" }}
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void ExecuteSelf() override {}

    void SetClosed(bool is_closed) { m_polyline.SetClosed(is_closed); }
    bool GetClosed() const { return m_polyline.GetClosed(); }

private:
    gs::Polyline m_polyline;

    RTTR_ENABLE(Node)

}; // Curve

}
}