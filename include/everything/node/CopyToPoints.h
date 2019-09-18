#pragma once

#include "everything/Node.h"

#include <SM_Vector.h>

namespace evt
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

    virtual void Execute(Evaluator& eval, TreeContext& ctx) override;

    void EnableUsePointDir(bool enable);

private:
    void CopyTo(const GeometryImpl& src, const GeometryImpl& dst);
    void CopyTo(const GeometryImpl& src, const GeometryImpl& dst, const sm::ivec3& norm_idx);
    void CopyTo(const GeometryImpl& src, const GeometryImpl& dst, const std::vector<sm::vec3>& norms);

private:
    bool m_use_pt_dir = false;

    RTTR_ENABLE(Node)

}; // CopyToPoints

}
}