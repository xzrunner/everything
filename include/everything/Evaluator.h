#pragma once

#include <painting0/RenderContext.h>

#include <vector>
#include <memory>

namespace pt3 { class RenderParams; }

namespace evt
{

class Node;

class Evaluator
{
public:
    static void Update(const std::vector<std::shared_ptr<Node>>& nodes);
    static void Draw(const pt0::RenderContext& rc, const std::vector<std::shared_ptr<Node>>& nodes);

private:
    static void Draw(const pt3::RenderParams& rp, const pt0::RenderContext& rc,
        const std::vector<std::shared_ptr<Node>>& nodes);

}; // Evaluator

}
