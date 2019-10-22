#pragma once

#include "sop/node/Geometry.h"

namespace sop
{
namespace node
{

class Blast;
class Output;

class Split : public Geometry
{
public:
    enum OutPortIdx
    {
        IDX_OUT_0 = 0,
        IDX_OUT_1,
    };

public:
    Split()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out0" }},
            {{ NodeVarType::Any, "out1" }},
        };

        InitSubNodes();
    }

    virtual void Execute(Evaluator& eval) override;

    void SetGroupName(const std::string& name);

private:
    void InitSubNodes();

private:
    std::string m_group_name;

    RTTR_ENABLE(Geometry)

}; // Split

}
}