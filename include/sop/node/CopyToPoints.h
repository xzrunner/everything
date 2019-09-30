#pragma once

#include "sop/Node.h"
#include "sop/GeoAttribute.h"

#include <SM_Vector.h>

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

    void SetSrcGroup(const std::string& group) { m_src_group = group; }
    void SetTargetGroup(const std::string& group) { m_target_group = group; }

    void EnableUsePointDir(bool enable);

private:
    void CopyTo(const GeometryImpl& src, const GeometryImpl& dst);
    void CopyTo(const GeometryImpl& src, const GeometryImpl& dst, const sm::ivec3& norm_idx);
    void CopyTo(const GeometryImpl& src, const GeometryImpl& dst, const std::vector<sm::vec3>& norms);

    void CopyTo(const GeometryImpl& src, const GeoAttribute::Point& dst);
    void CopyTo(const GeometryImpl& src, const GeoAttribute::Point& dst, const sm::ivec3& norm_idx);
    void CopyTo(const GeometryImpl& src, const GeoAttribute::Point& dst, const sm::vec3& norm);

private:
    std::string m_src_group;
    std::string m_target_group;

    bool m_use_pt_dir = false;

    RTTR_ENABLE(Node)

}; // CopyToPoints

}
}