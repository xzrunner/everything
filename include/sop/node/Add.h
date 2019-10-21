#pragma once

#include "sop/Node.h"
#include "sop/Group.h"

#include <SM_Vector.h>
#include <halfedge/TopoID.h>

namespace sop
{
namespace node
{

class Add : public Node
{
public:
    Add()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetPoints(const std::vector<sm::vec3>& points);

    void SetGroupName(const std::string& name);
    void SetGroupType(GroupType type);

private:
    std::vector<std::pair<he::TopoID, sm::vec3>>
        GetPrevVertices() const;

private:
    std::string m_group_name;
    GroupType   m_group_type = GroupType::GuessFromGroup;

    std::vector<sm::vec3> m_points;

    RTTR_ENABLE(Node)

}; // Add

}
}