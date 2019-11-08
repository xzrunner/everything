#pragma once

#include "sop/Node.h"

namespace sop
{
namespace node
{

class Dungeon : public Node
{
public:
    Dungeon()
    {
        m_exports = {
            {{ NodeVarType::Any, "out" }},
        };
    }

    virtual void Execute(Evaluator& eval) override;

    void SetSize(const sm::vec2& size);
    void SetSplitSize(const sm::vec2& split_sz);
    void SetMinSize(const sm::vec2& min_sz);

    void SetSeed(uint32_t seed);

private:
    void BuildModel();

private:
    sm::vec2 m_size     = sm::vec2(1, 1);
    sm::vec2 m_split_sz = sm::vec2(0.5f, 0.5f);
    sm::vec2 m_min_sz   = sm::vec2(0.25f, 0.25f);

    uint32_t m_seed = 0;

    RTTR_ENABLE(Node)

}; // Dungeon

}
}