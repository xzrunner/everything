#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class File : public Node
{
public:
    File()
    {
        m_imports = {
            {{ NodeVarType::Any, "in" }}
        };
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetFilepath(const std::string& filepath);

private:
    std::string m_filepath;

    RTTR_ENABLE(Node)

}; // File

}
}