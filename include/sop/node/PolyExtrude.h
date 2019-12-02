#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"
#include "sop/Widgets.h"

namespace he { struct Face; }

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
        std::vector<he::Face*>* new_faces);

    void AddToGroup(const std::string& group_name, const std::vector<he::Face*>& faces);

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/PolyExtrude.parm.h"
#include <hdiop/node_parms_gen.h>
#undef PARM_FILEPATH

}; // PolyExtrude

}
}