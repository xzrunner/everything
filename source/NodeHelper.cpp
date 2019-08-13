#include "everything/NodeHelper.h"
#include "everything/Node.h"

#include <model/ModelInstance.h>
#include <model/BrushBuilder.h>
#include <node0/SceneNode.h>
#include <node0/CompMaterial.h>
#include <node3/CompModelInst.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <node3/CompAABB.h>
#include <painting3/MaterialMgr.h>

#include <assert.h>

namespace evt
{

n0::SceneNodePtr
NodeHelper::GetInputSceneNode(const Node& node, int input_idx)
{
    auto& port = node.GetImports()[input_idx];
    if (port.conns.empty()) {
        return nullptr;
    }
    assert(port.conns.size() == 1);
    auto src_obj = port.conns[0].node.lock();
    if (src_obj) {
        return src_obj->GetSceneNode();
    } else {
        return nullptr;
    }
}

const model::BrushModel*
NodeHelper::GetBrushModel(const n0::SceneNode& node)
{
    assert(node.HasUniqueComp<n3::CompModelInst>());
    auto& src_cmodel = node.GetUniqueComp<n3::CompModelInst>();
    auto& model = src_cmodel.GetModel();
    auto& ext = model->GetModelExt();
    assert(ext && ext->Type() == model::EXT_BRUSH);
    return static_cast<model::BrushModel*>(ext.get());
}

pm3::PolytopePtr NodeHelper::GetPolytope(const n0::SceneNode& node)
{
    auto brush_model = NodeHelper::GetBrushModel(node);
    assert(brush_model);
    auto& brushes = brush_model->GetBrushes();
    assert(brushes.size() == 1);
    auto& brush = brushes[0];
    return brush.impl;
}

void NodeHelper::AddMaterialComp(n0::SceneNode& node)
{
    auto& cmaterial = node.AddUniqueComp<n0::CompMaterial>();
    auto mat = std::make_unique<pt0::Material>();
    typedef pt3::MaterialMgr::PhongUniforms UNIFORMS;
    mat->AddVar(UNIFORMS::ambient.name, pt0::RenderVariant(sm::vec3(0.04f, 0.04f, 0.04f)));
    mat->AddVar(UNIFORMS::diffuse.name, pt0::RenderVariant(sm::vec3(1, 1, 1)));
    mat->AddVar(UNIFORMS::specular.name, pt0::RenderVariant(sm::vec3(1, 1, 1)));
    mat->AddVar(UNIFORMS::shininess.name, pt0::RenderVariant(50.0f));
    cmaterial.SetMaterial(mat);
}

void NodeHelper::StoreBrush(n0::SceneNode& node, std::unique_ptr<model::ModelExtend>& model_ext)
{
    auto& cmodel_inst = node.GetUniqueComp<n3::CompModelInst>();
    cmodel_inst.GetModel()->SetModelExt(model_ext);
}

void NodeHelper::BuildPolymesh(n0::SceneNode& node, const model::BrushModel& brush_model)
{
    std::shared_ptr<model::Model> model =
        model::BrushBuilder::PolymeshFromBrush(brush_model);

    auto& cmodel = node.GetSharedComp<n3::CompModel>();
    cmodel.SetModel(model);

    auto& cmodel_inst = node.GetUniqueComp<n3::CompModelInst>();
    cmodel_inst.SetModel(model, 0);

    auto& caabb = node.GetUniqueComp<n3::CompAABB>();
    caabb.SetAABB(model->aabb);
}

// fixme: CompModel and CompModelInst shared same Model
//        should call BuildPolymesh()
//        Clone() only clone brush as ext, not clone Model
//void NodeHelper::UpdatePolymesh(n0::SceneNode& node, const model::BrushModel& brush_model)
//{
//    assert(node.HasUniqueComp<n3::CompModelInst>());
//    auto& src_cmodel_inst = node.GetUniqueComp<n3::CompModelInst>();
//    auto& model_inst = src_cmodel_inst.GetModel();
//    assert(model_inst);
//    auto model = model_inst->GetModel();
//
//    model::BrushBuilder::UpdateVBO(*model, brush_model);
//}

}