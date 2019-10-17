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

    virtual void Execute(Evaluator& eval) override;

    void SetEntityType(GroupType type);

    void SetFrameStyle(FrameStyle style);

    void SetNormalName(const std::string& name);
    void SetTangentName(const std::string& name);
    void SetBitangentName(const std::string& name);

private:
    void CreateAttrs();

    sm::vec3 CalcPointTangent(const GeoAttribute::Point& pt) const;

    std::vector<sm::vec3> CalcPointsNormal() const;
    std::vector<sm::vec3> CalcPointsTangent() const;

    std::vector<sm::vec3> CalcShapePointsTangent() const;
    std::vector<sm::vec3> CalcBrushPointsTangent() const;

    void AddToPointsAttr(const std::string& name, const std::vector<sm::vec3>& val);

private:
    // Primitives or Points
    GroupType m_entity_type = GroupType::Primitives;

    FrameStyle m_frame_style = FrameStyle::TwoEdges;

    std::string m_normal_name;
    std::string m_tangent_name;
    std::string m_bitangent_name;

    bool m_ortho = false;

    RTTR_ENABLE(Node)

}; // PolyFrame

}
}