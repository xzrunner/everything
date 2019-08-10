#pragma once

#include "everything/Node.h"

namespace pm3 { class Polytope; }

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
            {{ VariableType::Any, "in" }}
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    void SetGroupName(const std::string& name) { m_group_name = name; }
    void SetDistance(float dist);

private:
    static void ExtrudeFace(pm3::Polytope& poly, size_t face_idx, float dist);

private:
    std::string m_group_name;

    float m_distance = 0;

    RTTR_ENABLE(Node)

}; // PolyExtrude

}
}