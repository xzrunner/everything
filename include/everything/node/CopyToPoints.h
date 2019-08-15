#pragma once

#include "everything/Node.h"

#include <SM_Vector.h>

namespace pm3 { class Polytope; }
namespace model { class BrushModel; }

namespace evt
{
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
            {{ VariableType::Any, "in" }},
            {{ VariableType::Any, "pos" }}
        };
        m_exports = {
            {{ VariableType::Any, "out" }},
        };
    }

    virtual void Execute(TreeContext& ctx) override;

    void SetTransformUsingPointOrientations(bool enable);

private:
    std::unique_ptr<model::BrushModel>
        BuildBrush(const Geometry& src, const Geometry& dst) const;

    static std::unique_ptr<pm3::Polytope>
        CloneToPoint(const pm3::Polytope& src, const sm::vec3& target);

private:
    bool m_trans_with_point_dir = false;

    RTTR_ENABLE(Node)

}; // CopyToPoints

}
}