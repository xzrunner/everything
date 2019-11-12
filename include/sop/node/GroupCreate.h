#pragma once

#include "sop/Node.h"
#include "sop/Group.h"

#include <SM_Vector.h>

namespace sop
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

    enum PropID
    {
        DIR_X,
        DIR_Y,
        DIR_Z,

        MAX_BUILD_IN_PROP,
    };

    static const constexpr char* const PropNames[MAX_BUILD_IN_PROP] = {
        "dirx", "diry", "dirz"
    };

public:
    GroupCreate()
        : Node(MAX_BUILD_IN_PROP)
    {
        m_imports = {
            {{ NodeVarType::Any, "src" }},
            {{ NodeVarType::Any, "bounding" }},
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };

        InitProps();
    }

    virtual void Execute(Evaluator& eval) override;

    void  SetGroupName(const std::string& name);
    auto& GetGroupName() const { return m_group_name; }

    void SetGroupType(GroupType type);
    void SetGroupMerge(GroupMerge merge);

    // base group
    void EnableBaseGroup(const std::string& expr);
    void DisableBaseGroup();

    // keep in bounding regions
    void EnableKeepInBounding();
    void DisableKeepInBounding();

    // keep by normal
    void EnableKeepByNormals(const sm::vec3& direction, float spread_angle);
    void DisableKeepByNormals();

private:
    void InitProps();

    void SelectByNormals(Group& group);
    void SelectByBoundings(Group& group, std::shared_ptr<GeometryImpl>& bounding);
    void SelectAll(Group& group);

private:
    std::string m_group_name;
    GroupType   m_group_type = GroupType::Primitives;
    GroupMerge  m_merge_op   = GroupMerge::Replace;

    // base group
    bool        m_base_group = false;
    std::string m_base_group_expr;

    // keep in bounding regions
    bool m_keep_in_bounding = false;

    // keep by normals
    bool     m_keep_by_normals = false;
    float    m_spread_angle    = 180;

    RTTR_ENABLE(Node)

}; // GroupCreate

}
}