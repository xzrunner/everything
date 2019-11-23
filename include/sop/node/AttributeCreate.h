#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"
#include "sop/Group.h"

namespace sop
{
namespace node
{

class AttributeCreate : public Node
{
public:
    enum class ItemType
    {
        Float,
        Integer,
        Vector,
        String,
        FloatArray,
        IntegerArray,
        StringArray,
    };

    enum class ItemFltInfo
    {
        Guess,
        None,
        Position,
        Vector,
        Normal,
        Color,
        Quaternion,
        TransformMatrix,
        TextureCoordinate,
    };

public:
    AttributeCreate()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

private:
    bool IsNumberValid() const;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/AttributeCreate.parm.h"
#include "sop/node_parms_gen.h"
#undef PARM_FILEPATH

}; // AttributeCreate

}
}