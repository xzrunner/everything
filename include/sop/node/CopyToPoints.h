#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"

#include <SM_Matrix.h>

namespace sop
{

class GeometryImpl;
class ParmList;

namespace node
{

class CopyToPoints : public Node
{
public:
    enum InPortIdx
    {
        IDX_SRC_PRIM = 0,
        IDX_TARGET_POS,
    };

public:
    CopyToPoints()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }},
            {{ NodeVarType::Any, "pos" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

private:
    void CopyTo(const GeometryImpl& src, const GeometryImpl& dst);
    void CopyTo(const GeometryImpl& src, const GeometryImpl& dst, const ParmList& norm_list);
    void CopyTo(const GeometryImpl& src, const GeometryImpl& dst, const std::vector<sm::vec3>& norms);

    void CopyTo(const GeometryImpl& src, const GeoAttribute& dst, size_t dst_idx);
    void CopyTo(const GeometryImpl& src, const GeoAttribute& dst, size_t dst_idx, const sm::vec3& norm);

    sm::mat4 CalcMat(const GeoAttribute& attr, size_t p_idx, const sm::vec3& norm) const;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/CopyToPoints.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // CopyToPoints

}
}