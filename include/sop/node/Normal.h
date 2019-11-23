#pragma once

#include "sop/Node.h"
#include "sop/GeoAttrClass.h"
#include "sop/GeoAttribute.h"

namespace sop
{
namespace node
{

class Normal : public Node
{
public:
    enum class AddToType
    {
        Points,
        Vertices,
        Primitives,
        Detail
    };

public:
    Normal()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    static bool CalcSmoothedPointsNormal(const GeometryImpl& geo, std::vector<sm::vec3>& norms);

private:
    void AddToPoint();
    void AddToVertex();
    void AddToPrimitive();
    void AddToDetail();

    static sm::vec3 CalcPrimNorm(const GeoAttribute::Primitive& prim);

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/Normal.parm.h"
#include "sop/node_parms_gen.h"
#undef PARM_FILEPATH

}; // Normal

}
}