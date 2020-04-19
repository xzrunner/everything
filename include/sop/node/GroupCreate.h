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

    virtual void Execute(const ur2::Device& dev, Evaluator& eval) override;

private:
    void SelectByNormals(Group& group);
    void SelectByBoundings(Group& group, std::shared_ptr<GeometryImpl>& bounding);
    void SelectAll(Group& group);

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/GroupCreate.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // GroupCreate

}
}