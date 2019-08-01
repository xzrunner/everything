#include "everything/NodeHelper.h"
#include "everything/Node.h"

#include <model/ModelInstance.h>
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
    if (!sn) {
        return nullptr;
    }

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

const std::shared_ptr<pm3::Brush>
NodeHelper::GetBrush(const n0::SceneNodePtr& node)
{
    auto brush_model = GetBrushModel(node);
    if (!brush_model) {
        return nullptr;
    }

    auto& brushes = brush_model->GetBrushes();
    assert(brushes.size() == 1);
    return brushes[0].impl;
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

void NodeHelper::UpdateModelFromBrush(n0::SceneNode& node, const std::shared_ptr<pm3::Brush>& brush)
{
    std::vector<std::shared_ptr<pm3::Brush>> brushes;
    brushes.push_back(brush);
    std::shared_ptr<model::Model> model =
        model::BrushBuilder::PolymeshFromBrush(brushes);

    auto& cmodel = node.GetSharedComp<n3::CompModel>();
    cmodel.SetModel(model);

    auto& cmodel_inst = node.GetUniqueComp<n3::CompModelInst>();
    cmodel_inst.SetModel(model, 0);
}

}