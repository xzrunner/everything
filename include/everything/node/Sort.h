#pragma once

#include "everything/Node.h"

namespace evt
{
namespace node
{

class Sort : public Node
{
public:
    enum class Key
    {
        NoChange,
        X,
        Y,
        Z
    };

public:
    Sort()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    void SetKey(Key key);

private:
    Key m_key = Key::NoChange;

    RTTR_ENABLE(Node)

}; // Sort

}
}