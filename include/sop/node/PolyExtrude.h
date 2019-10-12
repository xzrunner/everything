#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"

namespace sop
{
namespace node
{

class PolyExtrude : public Node
{
public:
    PolyExtrude()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetGroupName(const std::string& name) { m_group_name = name; }

    void SetDistance(float dist);

    void SetOutputFront(bool output);
    void SetOutputBack(bool output);
    void SetOutputSide(bool output);

    void SetFrontGroupName(const std::string& name) { m_front_group = name; }
    void SetBackGroupName(const std::string& name) { m_back_group = name; }
    void SetSideGroupName(const std::string& name) { m_side_group = name; }

private:
    bool ExtrudeFace(GeoAttribute::Primitive& prim) const;

private:
    std::string m_group_name;

    float m_distance = 0;

    bool m_output_front = true;
    bool m_output_back  = false;
    bool m_output_side  = true;

    std::string m_front_group;
    std::string m_back_group;
    std::string m_side_group;

    RTTR_ENABLE(Node)

}; // PolyExtrude

}
}