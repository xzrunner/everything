#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"

namespace sop
{
namespace node
{

class Measure : public Node
{
public:
    enum class Type
    {
        Perimeter,
        Area,
        Curvature,
        Volume,
    };

public:
    Measure()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(const ur::Device& dev, Evaluator& eval) override;

private:
    void CalcPerimeter(std::string& name, std::vector<float>& val) const;
    void CalcArea(std::string& name, std::vector<float>& val) const;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Measure.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Measure

}
}