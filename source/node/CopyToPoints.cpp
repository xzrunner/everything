#include "everything/node/CopyToPoints.h"
#include "everything/NodeHelper.h"

#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>
#include <model/BrushBuilder.h>
#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <node3/CompTransform.h>

namespace evt
{
namespace node
{

void CopyToPoints::Execute(TreeContext& ctx)
{
    if (m_imports[IDX_SRC_PRIM].conns.empty() ||
        m_imports[IDX_TARGET_POS].conns.empty()) {
        return;
    }

    auto src_node = m_imports[IDX_SRC_PRIM].conns[0].node.lock();
    auto dst_node = m_imports[IDX_TARGET_POS].conns[0].node.lock();
    if (!src_node || !dst_node) {
        return;
    }

    auto src_obj = src_node->GetSceneNode();
    auto dst_obj = dst_node->GetSceneNode();
    if (!src_obj || !dst_obj) {
        return;
    }

    if (!m_scene_node)
    {
        m_scene_node = ns::NodeFactory::Create3D();

        m_scene_node->AddSharedComp<n3::CompModel>();
        m_scene_node->AddUniqueComp<n3::CompModelInst>();

        NodeHelper::AddMaterialComp(*m_scene_node);
    }

    auto brush_model = BuildBrush(*src_obj, *dst_obj);
    if (brush_model) {
        NodeHelper::BuildPolymesh(*m_scene_node, *brush_model);
        std::unique_ptr<model::ModelExtend> ext = std::move(brush_model);
        NodeHelper::StoreBrush(*m_scene_node, ext);
    }
}

void CopyToPoints::SetTransformUsingPointOrientations(bool enable)
{
    if (m_trans_with_point_dir == enable) {
        return;
    }

    m_trans_with_point_dir = enable;

    SetDirty(true);
}

std::unique_ptr<model::BrushModel>
CopyToPoints::BuildBrush(const n0::SceneNode& src, const n0::SceneNode& dst) const
{
    auto src_brush_model = NodeHelper::GetBrushModel(src);
    auto dst_brush_model = NodeHelper::GetBrushModel(dst);
    if (!src_brush_model || !dst_brush_model) {
        return nullptr;
    }

    auto& dst_ctrans = dst.GetUniqueComp<n3::CompTransform>();
    n3::CompTransform scaled_ctrans;
    scaled_ctrans.SetScale(dst_ctrans.GetScale());
    scaled_ctrans.SetAngle(dst_ctrans.GetAngle());
    scaled_ctrans.SetPosition(dst_ctrans.GetPosition());
    auto dst_mat = scaled_ctrans.GetTransformMat();

    auto brush_model = std::make_unique<model::BrushModel>();
    std::vector<model::BrushModel::Brush> brushes;
    for (auto& dst_brush : dst_brush_model->GetBrushes())
    {
        for (auto& pos : dst_brush.impl->Points())
        {
            auto target_pos = dst_mat * pos;
            for (auto& src_brush : src_brush_model->GetBrushes())
            {
                model::BrushModel::Brush dst;
                dst.desc = src_brush.desc;
                dst.impl = CloneToPoint(*src_brush.impl, target_pos);
                brushes.push_back(dst);
            }
        }
    }
    brush_model->SetBrushes(brushes);

    return brush_model;
}

std::unique_ptr<pm3::Polytope>
CopyToPoints::CloneToPoint(const pm3::Polytope& src, const sm::vec3& target)
{
    auto ret = std::make_unique<pm3::Polytope>(src);
    for (auto& v : ret->Points()) {
        v += target;
    }
    return ret;
}

}
}