#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Grid : public Node
{
public:
    Grid()
    {
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetSize(const sm::ivec2& size);
    void SetRows(size_t row);
    void SetColumns(size_t col);

private:
    void BuildModel();

private:
    sm::ivec2 m_size = sm::ivec2(10, 10);

    size_t m_rows    = 10;
    size_t m_columns = 10;

    RTTR_ENABLE(Node)

}; // Grid

}
}