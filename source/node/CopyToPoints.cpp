#include "everything/node/CopyToPoints.h"
#include "everything/node/Normal.h"

#include "everything/GeometryImpl.h"
#include "everything/NodeHelper.h"
#include "everything/GeoAttrHelper.h"

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

void CopyToPoints::Execute(Evaluator& eval)
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
        sm::ivec3 norm_idx;
        if (!GeoAttrHelper::QueryNormalIndices(dst_geo->GetAttr(), GeoAttrType::Point, norm_idx))
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
            CopyTo(*src_geo, *dst_geo, norm_idx);
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
    std::shared_ptr<Group> tar_group = nullptr;
    if (!m_target_group.empty())
    {
        tar_group = dst.GetGroup().Query(m_target_group);
        if (!tar_group || tar_group->type != GroupType::Points) {
            return;
        }
    }

    auto& pts = dst.GetAttr().GetPoints();
    if (tar_group) {
        for (auto i : tar_group->items) {
            CopyTo(src, *pts[i]);
        }
    } else {
        for (auto& p : pts) {
            CopyTo(src, *p);
        }
    }
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeometryImpl& dst, const sm::ivec3& norm_idx)
{
    std::shared_ptr<Group> tar_group = nullptr;
    if (!m_target_group.empty())
    {
        tar_group = dst.GetGroup().Query(m_target_group);
        if (!tar_group || tar_group->type != GroupType::Points) {
            return;
        }
    }

    auto& pts = dst.GetAttr().GetPoints();
    if (tar_group) {
        for (auto i : tar_group->items) {
            CopyTo(src, *pts[i], norm_idx);
        }
    } else {
        for (auto& p : pts) {
            CopyTo(src, *p, norm_idx);
        }
    }
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeometryImpl& dst, const std::vector<sm::vec3>& norms)
{
    std::shared_ptr<Group> tar_group = nullptr;
    if (!m_target_group.empty())
    {
        tar_group = dst.GetGroup().Query(m_target_group);
        if (!tar_group || tar_group->type != GroupType::Points) {
            return;
        }
    }

    auto& pts = dst.GetAttr().GetPoints();
    assert(pts.size() == norms.size());
    if (tar_group) {
        for (auto i : tar_group->items) {
            CopyTo(src, *pts[i], norms[i]);
        }
    } else {
        for (size_t i = 0, n = pts.size(); i < n; ++i) {
            CopyTo(src, *pts[i], norms[i]);
        }
    }
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeoAttribute::Point& dst)
{
    auto& attr = m_geo_impl->GetAttr();

    m_geo_impl->GetGroup().Combine(src.GetGroup(), attr.GetPoints().size(),
        attr.GetVertices().size(), attr.GetPrimtives().size());

    GeoAttribute src_attr(src.GetAttr());
    for (auto& p : src_attr.GetPoints()) {
        p->pos += dst.pos;
    }
    attr.Combine(src_attr);
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeoAttribute::Point& dst, const sm::ivec3& norm_idx)
{
    auto& attr = m_geo_impl->GetAttr();

    m_geo_impl->GetGroup().Combine(src.GetGroup(), attr.GetPoints().size(),
        attr.GetVertices().size(), attr.GetPrimtives().size());

    sm::vec3 norm;
    norm.x = dst.vars[norm_idx.x].f;
    norm.y = dst.vars[norm_idx.y].f;
    norm.z = dst.vars[norm_idx.z].f;

    auto rot = sm::mat4(sm::Quaternion::CreateFromVectors(BASE_DIR, norm));

    GeoAttribute src_attr(src.GetAttr());
    for (auto& p : src_attr.GetPoints()) {
        p->pos = rot * p->pos + dst.pos;
    }
    attr.Combine(src_attr);
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeoAttribute::Point& dst, const sm::vec3& norm)
{
    auto& attr = m_geo_impl->GetAttr();

    m_geo_impl->GetGroup().Combine(src.GetGroup(), attr.GetPoints().size(),
        attr.GetVertices().size(), attr.GetPrimtives().size());

    GeoAttribute src_attr(src.GetAttr());
    auto rot = sm::mat4(sm::Quaternion::CreateFromVectors(BASE_DIR, norm));
    for (auto& p : src_attr.GetPoints()) {
        p->pos = rot * p->pos + dst.pos;
    }
    attr.Combine(src_attr);
}

}
}