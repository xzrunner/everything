#pragma once

#include "sop/Node.h"
#include "sop/Group.h"
#include "sop/GeoAttribute.h"

namespace sop
{
namespace node
{

class PolyFrame : public Node
{
public:
    enum class FrameStyle
    {
        FirstEdge,
        TwoEdges,
        PrimitiveCentroid,
        TextureUV,
        TextureUVGradient,
        AttributeGradient,
        MikkT,
    };

public:
    PolyFrame()
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
    void CreateAttrs();

    sm::vec3 CalcPointTangent(const GeoAttribute::Point& pt) const;

    std::vector<sm::vec3> CalcPointsNormal() const;
    std::vector<sm::vec3> CalcPointsTangent() const;

    std::vector<sm::vec3> CalcShapePointsTangent() const;
    std::vector<sm::vec3> CalcBrushPointsTangent() const;

    void AddToPointsAttr(const std::string& name, const std::vector<sm::vec3>& val);

private:
    bool m_ortho = false;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/PolyFrame.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // PolyFrame

}
}