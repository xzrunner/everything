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
            {{ VariableType::Any, "in" }}
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

private:
    Key m_key;

    RTTR_ENABLE(Node)

}; // Sort

}
}