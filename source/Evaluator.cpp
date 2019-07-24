#include "everything/Evaluator.h"
#include "everything/Node.h"

#include <painting3/RenderSystem.h>
#include <node3/RenderSystem.h>

namespace evt
{

void Evaluator::Update(const std::vector<std::shared_ptr<Node>>& nodes)
{
}

void Evaluator::Draw(const pt0::RenderContext& rc, const std::vector<std::shared_ptr<Node>>& nodes)
{
    pt3::RenderParams rp;

    // draw face
    rp.type = pt3::RenderParams::DRAW_MESH;
    Draw(rp, rc, nodes);

    // draw edge
    rp.type = pt3::RenderParams::DRAW_BORDER_MESH;
    Draw(rp, rc, nodes);
}

void Evaluator::Draw(const pt3::RenderParams& rp, const pt0::RenderContext& rc,
                     const std::vector<std::shared_ptr<Node>>& nodes)
{
    for (auto& node : nodes)
    {
        node->Traverse([&](const n0::SceneNodePtr& n)->bool
        {
            n3::RenderSystem::Draw(*n, rp, rc);
            return true;
        });
    }
}

}