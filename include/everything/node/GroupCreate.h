#pragma once

#include "everything/Node.h"
#include "everything/node/typedef.h"

#include <SM_Vector.h>

namespace evt
{

struct BrushGroup;

namespace node
{

class GroupCreate : public Node
{
public:
    enum InPortIdx
    {
        SOURCE_OBJ_IDX = 0,
        BOUNDING_OBJ_IDX,
    };

public:
    GroupCreate()
    {
        m_imports = {
            {{ VariableType::Any, "src" }},
            {{ VariableType::Any, "bounding" }},
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;
    virtual void UpdateContext(TreeContext& ctx) override;

    void SetName(const std::string& name);
    void SetType(GroupType type);

    void EnableKeepByNormals(const sm::vec3& direction, float spread_angle);
    void DisableKeepByNormals();

    auto GetGroup() const { return m_group; }

private:
    void SelectByNormals();

private:
    std::string m_name;

    GroupType m_type = GroupType::Primitives;

    std::shared_ptr<BrushGroup> m_group = nullptr;

    // keep by normals
    bool     m_keep_by_normals = false;
    sm::vec3 m_direction       = sm::vec3(0, 0, 1);
    float    m_spread_angle    = 180;

    RTTR_ENABLE(Node)

}; // GroupCreate

}
}