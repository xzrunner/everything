#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"
#include "sop/Widgets.h"

#include <halfedge/HalfEdge.h>
#include <halfedge/Polyhedron.h>

namespace sop
{
namespace node
{

class PolyExtrude : public Node
{
public:
    enum class Extrusion
    {
        PrimEdgeNorm,
        PointNorm,
    };

public:
    PolyExtrude()
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
    bool ExtrudeFace(const std::vector<std::shared_ptr<GeoAttribute::Primitive>>& prims,
        std::vector<he::Polyhedron::Face>* new_faces);

    void AddToGroup(const std::string& group_name, const std::vector<he::Polyhedron::Face>& faces);

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/PolyExtrude.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // PolyExtrude

}
}