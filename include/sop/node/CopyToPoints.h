#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"

#include <SM_Matrix.h>

namespace sop
{

class GeometryImpl;

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
    void CopyTo(const GeometryImpl& src, const GeometryImpl& dst, size_t norm_idx);
    void CopyTo(const GeometryImpl& src, const GeometryImpl& dst, const std::vector<sm::vec3>& norms);

    void CopyTo(const GeometryImpl& src, const GeoAttribute::Point& dst,
        const std::vector<GeoAttribute::VarDesc>& desc);
    void CopyTo(const GeometryImpl& src, const GeoAttribute::Point& dst,
        const std::vector<GeoAttribute::VarDesc>& desc, const sm::vec3& norm);

    sm::mat4 CalcMat(const GeoAttribute::Point& pt, const std::vector<GeoAttribute::VarDesc>& desc,
        const sm::vec3& norm) const;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "sop/node/CopyToPoints.parm.h"
#include "sop/node_parms_gen.h"
#undef PARM_FILEPATH

}; // CopyToPoints

}
}