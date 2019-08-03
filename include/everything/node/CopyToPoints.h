#pragma once

#include "everything/Node.h"

#include <SM_Vector.h>

namespace pm3 { struct Brush; }

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

    void SetTransformUsingPointOrientations(bool enable);

protected:
    virtual void ExecuteSelf() override;

private:
    static std::unique_ptr<pm3::Brush>
        CloneToPoint(const pm3::Brush& src, const sm::vec3& target);

private:
    bool m_trans_with_point_dir = false;

    RTTR_ENABLE(Node)

}; // CopyToPoints

}
}