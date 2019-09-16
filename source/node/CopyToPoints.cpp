#include "everything/node/CopyToPoints.h"
#include "everything/GeometryImpl.h"
#include "everything/NodeHelper.h"

namespace evt
{
namespace node
{

void CopyToPoints::Execute(Evaluator& eval, TreeContext& ctx)
{
    m_geo_impl.reset();

    auto src_geo = NodeHelper::GetInputGeo(*this, IDX_SRC_PRIM);
    auto dst_geo = NodeHelper::GetInputGeo(*this, IDX_TARGET_POS);
    if (!src_geo || !dst_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Brush);

    auto& attr = m_geo_impl->GetAttr();
    for (auto& move_to : dst_geo->GetAttr().GetPoints())
    {
        GeoAttribute src_attr(src_geo->GetAttr());
        for (auto& p : src_attr.GetPoints()) {
            p->pos += move_to->pos;
        }
        attr.Combine(src_attr);
    }

    m_geo_impl->UpdateByAttr();
}

void CopyToPoints::EnableUsePointDir(bool enable)
{
    if (m_use_pt_dir == enable) {
        return;
    }

    m_use_pt_dir = enable;

    SetDirty(true);
}

}
}