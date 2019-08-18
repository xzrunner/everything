#pragma once

#include "everything/Node.h"
#include "everything/Group.h"

#include <SM_Vector.h>

namespace evt
{
namespace node
{

class GroupCreate : public Node
{
public:
    enum InPortIdx
    {
        IDX_SOURCE_OBJ = 0,
        IDX_BOUNDING_OBJ,
    };

public:
    GroupCreate()
    {
        m_imports = {
            {{ NodeVarType::Any, "src" }},
            {{ NodeVarType::Any, "bounding" }},
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;
    virtual void UpdateContext(TreeContext& ctx) override;

    void  SetName(const std::string& name);
    auto& GetName() const { return m_name; }

    void SetType(GroupType type);

    void EnableKeepByNormals(const sm::vec3& direction, float spread_angle);
    void DisableKeepByNormals();

private:
    void SelectByNormals(Group& group);

private:
    std::string m_name;

    GroupType m_type = GroupType::Primitives;

    // keep by normals
    bool     m_keep_by_normals = false;
    sm::vec3 m_direction       = sm::vec3(0, 0, 1);
    float    m_spread_angle    = 180;

    RTTR_ENABLE(Node)

}; // GroupCreate

}
}