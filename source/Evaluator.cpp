#include "everything/Evaluator.h"
#include "everything/Node.h"

#include <painting3/RenderSystem.h>
#include <painting3/MaterialMgr.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <node3/RenderSystem.h>

namespace evt
{

void Evaluator::Update(const std::vector<std::shared_ptr<Node>>& nodes)
{
}

void Evaluator::Draw(const std::vector<std::shared_ptr<Node>>& nodes)
{
    pt3::RenderParams params;
    params.type = pt3::RenderParams::DRAW_MESH;

    pt0::RenderContext ctx;
    ctx.AddVar(
        pt3::MaterialMgr::PositionUniforms::light_pos.name,
        pt0::RenderVariant(sm::vec3(0, 2, -4))
    );
    auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
    assert(wc);
    ctx.AddVar(
        pt3::MaterialMgr::PosTransUniforms::view.name,
        pt0::RenderVariant(wc->GetViewMat())
    );
    ctx.AddVar(
        pt3::MaterialMgr::PosTransUniforms::projection.name,
        pt0::RenderVariant(wc->GetProjMat())
    );

    for (auto& node : nodes)
    {
        node->Traverse([&](const n0::SceneNodePtr& n)->bool
        {
            n3::RenderSystem::Draw(*n, params, ctx);
            return true;
        });
    }
}

}