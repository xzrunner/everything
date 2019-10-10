#pragma once

#include "sop/Node.h"
#include "sop/GeoAttrClass.h"

namespace sop
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

    virtual void Execute(Evaluator& eval) override;

    void SetAttrAddTo(GeoAttrClass cls);

    static std::vector<sm::vec3> CalcBrushPointsNormal(const GeometryImpl& geo);

private:
    void AddToPoint();
    void AddToVertex();
    void AddToPrimitive();
    void AddToDetail();

private:
    GeoAttrClass m_attr_add_to = GeoAttrClass::Vertex;

    RTTR_ENABLE(Node)

}; // Normal

}
}