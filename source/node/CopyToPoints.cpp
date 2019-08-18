#include "everything/node/CopyToPoints.h"
#include "everything/GeometryNode.h"

#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>

namespace evt
{
namespace node
{

void CopyToPoints::Execute(TreeContext& ctx)
{
    m_geo.reset();

    auto src_geo = GetInputGeo(IDX_SRC_PRIM);
    auto dst_geo = GetInputGeo(IDX_TARGET_POS);
    if (!src_geo || !dst_geo) {
        return;
    }

    m_geo = std::make_shared<GeometryNode>(GeometryNode::DataType::Brush);

    auto brush_model = BuildBrush(*src_geo, *dst_geo);
    if (brush_model) {
        m_geo->UpdateModel(*brush_model);
        m_geo->StoreBrush(brush_model);
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
CopyToPoints::BuildBrush(const GeometryNode& src, const GeometryNode& dst) const
{
    auto src_brush_model = src.GetBrushModel();
    if (!src_brush_model) {
        return nullptr;
    }
    auto& src_brushes = src_brush_model->GetBrushes();
    if (src_brushes.empty()) {
        return nullptr;
    }

    auto brush_model = std::make_unique<model::BrushModel>();
    std::vector<model::BrushModel::Brush> brushes;
    dst.TraversePoints([&](sm::vec3& v, bool& dirty)->bool
    {
        for (auto& src_brush : src_brushes)
        {
            model::BrushModel::Brush dst;
            dst.desc = src_brush.desc;
            dst.impl = CloneToPoint(*src_brush.impl, v);
            brushes.push_back(dst);
        }
        dirty = false;
        return true;
    });
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