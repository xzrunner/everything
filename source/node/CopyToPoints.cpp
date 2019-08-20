#include "everything/node/CopyToPoints.h"
#include "everything/Geometry.h"

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

    m_geo = std::make_shared<Geometry>(GeoShapeType::Faces);

    auto& attr = m_geo->GetAttr();
    for (auto& move_to : dst_geo->GetAttr().GetPoints())
    {
        GeoAttribute src_attr(src_geo->GetAttr());
        for (auto& p : src_attr.GetPoints()) {
            p->pos += move_to->pos;
        }
        attr.Combine(src_attr);
    }
    attr.ResetPointsOrder();

    m_geo->UpdateByAttr();
}

void CopyToPoints::SetTransformUsingPointOrientations(bool enable)
{
    if (m_trans_with_point_dir == enable) {
        return;
    }

    m_trans_with_point_dir = enable;

    SetDirty(true);
}

}
}