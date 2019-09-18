#include "everything/node/CopyToPoints.h"
#include "everything/node/Normal.h"

#include "everything/GeometryImpl.h"
#include "everything/NodeHelper.h"
#include "everything/GeoAttrName.h"

#include <SM_Quaternion.h>
#include <SM_Matrix.h>

namespace
{

const sm::vec3 BASE_DIR(0, 0, 1);

}

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

    if (!m_use_pt_dir)
    {
        CopyTo(*src_geo, *dst_geo);
    }
    else
    {
        auto& desc = dst_geo->GetAttr().GetAttrDesc(GeoAttrType::Point);
        sm::ivec3 idx(-1, -1, -1);
        for (int i = 0, n = desc.size(); i < n; ++i)
        {
            if (desc[i].name == GeoAttrName::norm_x) {
                assert(desc[i].type == VarType::Float);
                idx.x = i;
            }
            if (desc[i].name == GeoAttrName::norm_y) {
                assert(desc[i].type == VarType::Float);
                idx.y = i;
            }
            if (desc[i].name == GeoAttrName::norm_z) {
                assert(desc[i].type == VarType::Float);
                idx.z = i;
            }
        }
        if (idx.x < 0 || idx.y < 0 || idx.z < 0)
        {
            auto dst_type = dst_geo->GetAdaptorType();
            switch (dst_type)
            {
            case GeoAdaptor::Type::Shape:
                CopyTo(*src_geo, *dst_geo);
                break;
            case GeoAdaptor::Type::Brush:
                CopyTo(*src_geo, *dst_geo, Normal::CalcBrushPointsNormal(*dst_geo));
                break;
            default:
                assert(0);
            }
        }
        else
        {
            CopyTo(*src_geo, *dst_geo, idx);
        }
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

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeometryImpl& dst)
{
    auto& attr = m_geo_impl->GetAttr();
    auto& to_attr = dst.GetAttr();
    for (auto& to_p : to_attr.GetPoints())
    {
        GeoAttribute src_attr(src.GetAttr());
        for (auto& p : src_attr.GetPoints()) {
            p->pos += to_p->pos;
        }
        attr.Combine(src_attr);
    }
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeometryImpl& dst, const sm::ivec3& norm_idx)
{
    auto& attr = m_geo_impl->GetAttr();
    auto& to_attr = dst.GetAttr();
    for (auto& to_p : to_attr.GetPoints())
    {
        sm::vec3 norm;
        norm.x = to_p->vars[norm_idx.x].f;
        norm.y = to_p->vars[norm_idx.y].f;
        norm.z = to_p->vars[norm_idx.z].f;

        auto rot = sm::mat4(sm::Quaternion::CreateFromVectors(BASE_DIR, norm));

        GeoAttribute src_attr(src.GetAttr());
        for (auto& p : src_attr.GetPoints()) {
            p->pos = rot * p->pos + to_p->pos;
        }
        attr.Combine(src_attr);
    }
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeometryImpl& dst, const std::vector<sm::vec3>& norms)
{
    auto& attr = m_geo_impl->GetAttr();
    auto& to_points = dst.GetAttr().GetPoints();
    assert(to_points.size() == norms.size());
    for (size_t i = 0, n = to_points.size(); i < n; ++i)
    {
        GeoAttribute src_attr(src.GetAttr());
        auto rot = sm::mat4(sm::Quaternion::CreateFromVectors(BASE_DIR, norms[i]));
        for (auto& p : src_attr.GetPoints()) {
            p->pos = rot * p->pos + to_points[i]->pos;
        }
        attr.Combine(src_attr);
    }
}

}
}