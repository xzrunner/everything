#pragma once

#include "everything/Node.h"

#include <SM_Vector.h>
#include <polymesh3/Geometry.h>

namespace evt
{
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

    void  SetName(const std::string& name);
    auto& GetName() const { return m_name; }

    void SetType(pm3::GroupType type);

    void EnableKeepByNormals(const sm::vec3& direction, float spread_angle);
    void DisableKeepByNormals();

private:
    void SelectByNormals(pm3::Group& group);

private:
    std::string m_name;

    pm3::GroupType m_type = pm3::GroupType::Face;

    // keep by normals
    bool     m_keep_by_normals = false;
    sm::vec3 m_direction       = sm::vec3(0, 0, 1);
    float    m_spread_angle    = 180;

    RTTR_ENABLE(Node)

}; // GroupCreate

}
}