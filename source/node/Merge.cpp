#include "everything/node/Merge.h"
#include "everything/NodeHelper.h"

#include <polymesh3/Brush.h>
#include <model/BrushModel.h>
#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>

namespace evt
{
namespace node
{

void Merge::ExecuteSelf()
{
    if (!m_scene_node)
    {
        m_scene_node = ns::NodeFactory::Create3D();

        m_scene_node->AddSharedComp<n3::CompModel>();
        m_scene_node->AddUniqueComp<n3::CompModelInst>();

        NodeHelper::AddMaterialComp(*m_scene_node);
    }

    std::vector<n0::SceneNodePtr> children;
    for (auto& port : m_imports)
    {
        if (port.conns.empty()) {
            continue;
        }

        assert(port.conns.size() == 1);
        auto src_obj = port.conns[0].node.lock();
        if (!src_obj) {
            continue;
        }

        children.push_back(src_obj->GetSceneNode());
    }

    // build brush
    auto brush_model = std::make_unique<model::BrushModel>();
    std::vector<model::BrushModel::Brush> brushes;
    for (auto& c : children)
    {
        assert(c);
        auto brush_model = NodeHelper::GetBrushModel(*c);
        if (!brush_model) {
            continue;
        }
        for (auto& b : brush_model->GetBrushes()) {
            brushes.push_back({ b.desc, std::make_shared<pm3::Brush>(*b.impl) });
        }
    }
    brush_model->SetBrushes(brushes);

    // build model
    NodeHelper::UpdateModelFromBrush(*m_scene_node, *brush_model);

    std::unique_ptr<model::ModelExtend> ext = std::move(brush_model);
    auto& cmodel_inst = m_scene_node->GetUniqueComp<n3::CompModelInst>();
    cmodel_inst.GetModel()->SetModelExt(ext);
}

}
}