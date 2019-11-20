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
    enum class BoundingType
    {
        Box,
        Sphere,
        Object,
        Volume,
    };

    enum InPortIdx
    {
        IDX_SOURCE_OBJ = 0,
        IDX_BOUNDING_OBJ,
    };

    enum PropID
    {
        NAME,
        TYPE,
        MERGE,

        // base
        BASE_ON,
        BASE_GROUP,

        // bounding
        BOUNDING_ON,

        // normals
        NORMAL_ON,
        DIR,
        ANGLE,

        MAX_BUILD_IN_PROP,
    };

    static const constexpr char* const PropNames[MAX_BUILD_IN_PROP] = {
        "groupname", "groupbase", "mergeop",
        "groupbase", "basegroup",
        "groupbounding",
        "groupnormal", "dir", "angle",
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

    void SetGroupName(const std::string& name);

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

    GroupMerge GetMergeOP() const;

    RTTR_ENABLE(Node)

}; // GroupCreate

}
}