#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Switch : public Node
{
public:
    enum PropID
    {
        SELECTED,

        MAX_BUILD_IN_PROP,
    };

    static const constexpr char* const PropNames[MAX_BUILD_IN_PROP] = {
        "input",
    };

public:
    Switch()
        : Node(MAX_BUILD_IN_PROP)
    {
        m_imports = {
            {{ NodeVarType::Any, "in0" }},
            {{ NodeVarType::Any, "in1" }},
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };

        InitProps();
    }

    virtual void Execute(Evaluator& eval) override;

    virtual void AddInputPorts(size_t num) override;

    void SetSelected(size_t select);

private:
    void InitProps();

    RTTR_ENABLE(Node)

}; // Switch

}
}