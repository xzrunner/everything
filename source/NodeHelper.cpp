#include "everything/NodeHelper.h"
#include "everything/Node.h"

#include <model/ModelInstance.h>
#include <model/BrushModel.h>
#include <model/BrushBuilder.h>
#include <node0/SceneNode.h>
#include <node3/CompModelInst.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>

#include <assert.h>

namespace evt
{

n0::SceneNodePtr
NodeHelper::ClonePrevSceneObj(const Node& node, int prev_idx)
{
    auto& port = node.GetImports()[prev_idx];
    if (port.conns.empty()) {
        return nullptr;
    }
    auto src_obj = port.conns[0].node.lock();
    if (!src_obj) {
        return nullptr;
    }

    auto sn = src_obj->GetSceneNode();
    return sn->Clone();
}

const model::BrushModel*
NodeHelper::GetBrushModel(const n0::SceneNodePtr& node)
{
    assert(node && node->HasUniqueComp<n3::CompModelInst>());
    auto& src_cmodel = node->GetUniqueComp<n3::CompModelInst>();
    auto& model = src_cmodel.GetModel();
    auto& ext = model->GetModelExt();
    assert(ext && ext->Type() == model::EXT_BRUSH);
    return static_cast<model::BrushModel*>(ext.get());
}

void NodeHelper::UpdateModelFromBrush(n0::SceneNode& node, const model::BrushModel& brush)
{
    std::shared_ptr<model::Model> model =
        model::BrushBuilder::PolymeshFromBrush(brush);

    auto& cmodel = node.GetSharedComp<n3::CompModel>();
    cmodel.SetModel(model);

    auto& cmodel_inst = node.GetUniqueComp<n3::CompModelInst>();
    cmodel_inst.SetModel(model, 0);
}

}