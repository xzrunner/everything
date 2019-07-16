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
            {{ VariableType::Port, "prev" }}
        };
        m_exports = {
            {{ VariableType::Port, "next" }}
        };
    }

    //virtual void Execute() override;
    //virtual void Traverse(std::function<bool(const n0::SceneNodePtr&)> func) const override {}

    void SetClosed(bool is_closed) { m_polyline.SetClosed(is_closed); }
    bool GetClosed() const { return m_polyline.GetClosed(); }

private:
    gs::Polyline m_polyline;

    RTTR_ENABLE(Node)

}; // Curve

}
}