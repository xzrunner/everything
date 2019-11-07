#pragma once

#include "sop/Node.h"

#include <SM_Vector.h>

namespace sop
{
namespace node
{

class UVQuickShade : public Node
{
public:
    UVQuickShade()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetImageFile(const std::string& image_file);

private:
    std::string m_image_file;

    int m_diffuse_tex = -1;

    RTTR_ENABLE(Node)

}; // UVQuickShade

}
}