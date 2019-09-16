#pragma once

#include "everything/Node.h"
#include "everything/GeoAttrType.h"

namespace evt
{
namespace node
{

class Normal : public Node
{
public:
    Normal()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval, TreeContext& ctx) override;

    void SetAttrAddNormalTo(GeoAttrType attr);

private:
    void AddToPoint();
    void AddToVertex();
    void AddToPrimitive();
    void AddToDetail();

private:
    GeoAttrType m_attr_add_norm_to = GeoAttrType::Vertex;

    RTTR_ENABLE(Node)

}; // Normal

}
}