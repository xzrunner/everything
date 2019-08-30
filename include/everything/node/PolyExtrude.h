#pragma once

#include "everything/Node.h"
#include "everything/GeoAttribute.h"

namespace evt
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

    virtual void Execute(Evaluator& eval, TreeContext& ctx) override;

    void SetGroupName(const std::string& name) { m_group_name = name; }
    void SetDistance(float dist);

private:
    void ExtrudeFace(GeoAttribute::Primitive& prim) const;

private:
    std::string m_group_name;

    float m_distance = 0;

    RTTR_ENABLE(Node)

}; // PolyExtrude

}
}