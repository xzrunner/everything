#pragma once

#include "everything/Node.h"

namespace pm3 { struct Brush; }

namespace evt
{

struct BrushGroup;

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

    virtual void BeforeUpdateContext() override;

    void SetGroupName(const std::string& name) { m_group_name = name; }
    void SetDistance(float dist);

private:
    virtual void ExecuteSelf() override;
    virtual void UpdateCtxSelf(TreeContext& ctx) override;

private:
    static void ExtrudeFace(pm3::Brush& brush, size_t face_idx, float dist);

private:
    std::string m_group_name;

    float m_distance = 0;

    std::shared_ptr<BrushGroup> m_group = nullptr;

    RTTR_ENABLE(Node)

}; // PolyExtrude

}
}