#include "sop/node/CopyToPoints.h"
#include "sop/node/Normal.h"

#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/ParmList.h"

#include <SM_Quaternion.h>

namespace
{

const sm::vec3 Z_DIR(0, 0, 1);

sm::mat4 BuildRotMat(const sm::vec3& norm, const sm::vec3& up)
{
    auto z = norm.Normalized();
    auto y = up.Normalized();
    auto x = norm.Cross(up).Normalized();
    y = z.Cross(x).Normalized();

    sm::mat4 mat;
    float* m = mat.x;
    m[0] = x.x; m[4] = y.x; m[8]  = z.x; m[12] = 0;
    m[1] = x.y; m[5] = y.y; m[9]  = z.y; m[13] = 0;
    m[2] = x.z; m[6] = y.z; m[10] = z.z; m[14] = 0;
    m[3] = 0;   m[7] = 0;   m[11] = 0;   m[15] = 1.0;
    return mat;
}

}

namespace sop
{
namespace node
{

void CopyToPoints::Execute(const ur::Device& dev, Evaluator& eval)
{
    m_geo_impl.reset();

    auto src_geo = NodeHelper::GetInputGeo(*this, IDX_SRC_PRIM);
    auto dst_geo = NodeHelper::GetInputGeo(*this, IDX_TARGET_POS);
    if (!src_geo || !dst_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(dev, GeoAdaptor::Type::Brush);

    if (!m_trans_use_pt_dir)
    {
        CopyTo(*src_geo, *dst_geo);
    }
    else
    {
        auto norm_list = dst_geo->GetAttr().QueryParmList(GeoAttrClass::Point, GEO_ATTR_NORM);
        if (!norm_list)
        {
            auto dst_type = dst_geo->GetAdaptorType();
            switch (dst_type)
            {
            case GeoAdaptor::Type::Shape:
                CopyTo(*src_geo, *dst_geo);
                break;
            case GeoAdaptor::Type::Brush:
            {
                std::vector<sm::vec3> norms;
                bool succ = Normal::CalcSmoothedPointsNormal(*dst_geo, norms);
                assert(succ);
                CopyTo(*src_geo, *dst_geo, norms);
            }
                break;
            default:
                assert(0);
            }
        }
        else
        {
            CopyTo(*src_geo, *dst_geo, *norm_list);
        }
    }

    m_geo_impl->UpdateByAttr(dev);
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeometryImpl& dst)
{
    auto& pts = dst.GetAttr().GetPoints();

    std::shared_ptr<Group> tar_group = nullptr;
    if (!m_target_group.empty())
    {
        tar_group = dst.GetGroup().Query(m_target_group, pts.size(), GroupType::Points);
        if (!tar_group) {
            return;
        }
    }

    if (tar_group) {
        for (auto i : tar_group->GetItems()) {
            CopyTo(src, dst.GetAttr(), i);
        }
    } else {
        for (size_t i = 0, n = pts.size(); i < n; ++i) {
            CopyTo(src, dst.GetAttr(), i);
        }
    }
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeometryImpl& dst, const ParmList& norm_list)
{
    auto& pts = dst.GetAttr().GetPoints();

    std::shared_ptr<Group> tar_group = nullptr;
    if (!m_target_group.empty())
    {
        tar_group = dst.GetGroup().Query(m_target_group, pts.size(), GroupType::Points);
        if (!tar_group) {
            return;
        }
    }

    assert(norm_list.GetType() == ParmType::Vector);
    auto& data = static_cast<const ParmFlt3List&>(norm_list).GetAllItems();
    if (tar_group) {
        for (auto i : tar_group->GetItems()) {
            CopyTo(src, dst.GetAttr(), i, data[i]);
        }
    } else {
        for (size_t i = 0, n = pts.size(); i < n; ++i) {
            CopyTo(src, dst.GetAttr(), i, data[i]);
        }
    }
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeometryImpl& dst, const std::vector<sm::vec3>& norms)
{
    auto& pts = dst.GetAttr().GetPoints();

    std::shared_ptr<Group> tar_group = nullptr;
    if (!m_target_group.empty())
    {
        tar_group = dst.GetGroup().Query(m_target_group, pts.size(), GroupType::Points);
        if (!tar_group) {
            return;
        }
    }

    assert(pts.size() == norms.size());
    if (tar_group) {
        for (auto i : tar_group->GetItems()) {
            CopyTo(src, dst.GetAttr(), i, norms[i]);
        }
    } else {
        for (size_t i = 0, n = pts.size(); i < n; ++i) {
            CopyTo(src, dst.GetAttr(), i, norms[i]);
        }
    }
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeoAttribute& dst, size_t dst_idx)
{
    auto& attr = m_geo_impl->GetAttr();

    m_geo_impl->GetGroup().Combine(src.GetGroup(), attr.GetPoints().size(),
        attr.GetVertices().size(), attr.GetPrimtives().size());

    sm::vec3 norm;
    norm.MakeInvalid();
    auto mt = CalcMat(dst, dst_idx, norm);

    GeoAttribute src_attr(src.GetAttr());
    for (auto& p : src_attr.GetPoints()) {
        p->pos = mt * p->pos;
    }
    attr.Combine(src_attr);
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeoAttribute& dst, size_t dst_idx, const sm::vec3& norm)
{
    auto& attr = m_geo_impl->GetAttr();

    m_geo_impl->GetGroup().Combine(src.GetGroup(), attr.GetPoints().size(),
        attr.GetVertices().size(), attr.GetPrimtives().size());

    auto mt = CalcMat(dst, dst_idx, norm);

    GeoAttribute src_attr(src.GetAttr());
    auto rot = sm::mat4(sm::Quaternion::CreateFromVectors(Z_DIR, norm));
    for (auto& p : src_attr.GetPoints()) {
        p->pos = mt * p->pos;
    }
    attr.Combine(src_attr);
}

/*
If pivot exists, use it as the local transformation of the copy/instance.

If the transform attribute exists:

Use it as a 3¡Á3 or 4¡Á4 matrix to transform the copy/instance.

If the transform attribute does not exist:

If the orient attribute exists:

Use it to orient the copy/instance.

If the orient attributes does not exist:

Orient the copy/instance using N as the +Z axis and up as +Y axis.

If N does not exist, use v (velocity) if it exists.

If the rot attribute exists, apply it after the above.

If pscale exists, use it to scale the to scale the copy/instance (multiplied by scale if it exists).

If scale exists, use it to scale the copy/instance (multiplied by pscale if it exists).

If trans exists, use it and P to move the copy/instance.

Key:
X = pivot matrix (translate by -pivot)
O = orient matrix
S = scale matrix (scale * pscale)
L = alignment matrix (*)
R = rot matrix
T = trans matrix (trans + P)
M = transform matrix

(*) The alignment matrix (L) is defined by N or v and up.

IF N exists AND up exists and isn't {0,0,0}:
   L = mlookatup(N,0,up)
ELSE IF N exists:
   L = dihedral({0,0,1},N)
ELSE IF v exists AND up exists and isn't {0,0,0}:
   L = mlookatup(v,0,up)
ELSE IF v exists:
   L = dihedral({0,0,1},v)

IF transform exists:
   Transform = X*M*T
ELSE IF orient exists:
   Transform = X*S*(O*R)*T
ELSE:
   Transform = X*S*L*R*T
*/

sm::mat4 CopyToPoints::CalcMat(const GeoAttribute& attr, size_t p_idx, const sm::vec3& norm) const
{
    sm::mat4 mt;

    sm::mat4 x_mt;
    sm::mat4 o_mt;
    sm::mat4 s_mt;
    sm::mat4 l_mt;
    sm::mat4 r_mt;
    sm::mat4 t_mt;
    sm::mat4 m_mt;

    // pivot matrix (translate by -pivot)
    auto pivot_list = attr.QueryParmList(GeoAttrClass::Point, GEO_ATTR_PIVOT);
    if (pivot_list && pivot_list->GetType() == ParmType::Vector)
    {
        assert(pivot_list->Size() > p_idx);
        auto& pivot = std::static_pointer_cast<ParmFlt3List>(pivot_list)->GetAllItems()[p_idx];
        x_mt = sm::mat4::Translated(-pivot.x, -pivot.y, -pivot.z);
    }

    // orient matrix
    auto orient_list = attr.QueryParmList(GeoAttrClass::Point, GEO_ATTR_ORIENT);
    if (orient_list && orient_list->GetType() == ParmType::Float4)
    {
        assert(orient_list->Size() > p_idx);
        auto& orient = std::static_pointer_cast<ParmFlt4List>(orient_list)->GetAllItems()[p_idx];
        o_mt = sm::mat4(sm::Quaternion(orient.x, orient.y, orient.z, orient.w));
    }

    // scale matrix (scale * pscale)
    auto pscale_list = attr.QueryParmList(GeoAttrClass::Point, GEO_ATTR_PSCALE);
    if (pscale_list && pscale_list->GetType() == ParmType::Float)
    {
        assert(pscale_list->Size() > p_idx);
        auto& pscale = std::static_pointer_cast<ParmFltList>(pscale_list)->GetAllItems()[p_idx];
        s_mt = sm::mat4::Scaled(pscale, pscale, pscale) * s_mt;
    }
    auto scale_list = attr.QueryParmList(GeoAttrClass::Point, GEO_ATTR_SCALE);
    if (scale_list && scale_list->GetType() == ParmType::Vector)
    {
        assert(scale_list->Size() > p_idx);
        auto& scale = std::static_pointer_cast<ParmFlt3List>(scale_list)->GetAllItems()[p_idx];
        s_mt = sm::mat4::Scaled(scale.x, scale.y, scale.z) * s_mt;
    }

    // alignment matrix, defined by N or v and up.
    sm::vec3 N, v, up;
    auto norm_list = attr.QueryParmList(GeoAttrClass::Point, GEO_ATTR_NORM);
    if (norm_list && norm_list->GetType() == ParmType::Vector) {
        assert(p_idx < norm_list->Size());
        N = std::static_pointer_cast<ParmFlt3List>(norm_list)->GetAllItems()[p_idx];
    } else {
        N = norm;
    }
    auto spd_list = attr.QueryParmList(GeoAttrClass::Point, GEO_ATTR_SPD);
    if (spd_list && spd_list->GetType() == ParmType::Vector) {
        assert(p_idx < spd_list->Size());
        v = std::static_pointer_cast<ParmFlt3List>(spd_list)->GetAllItems()[p_idx];
    } else {
        v.MakeInvalid();
    }
    auto up_list = attr.QueryParmList(GeoAttrClass::Point, GEO_ATTR_UP);
    if (up_list && up_list->GetType() == ParmType::Vector) {
        assert(p_idx < up_list->Size());
        up = std::static_pointer_cast<ParmFlt3List>(up_list)->GetAllItems()[p_idx];
    } else {
        up.MakeInvalid();
    }
    if (N.IsValid() && up.IsValid() && N != sm::vec3(0, 0, 0) && up != sm::vec3(0, 0, 0)) {
//        l_mt = sm::mat4::LookAt(sm::vec3(0, 0, 0), N, up).Inverted();
        l_mt = BuildRotMat(N, up);
    } else if (N.IsValid()) {
        l_mt = sm::mat4(sm::Quaternion::CreateFromVectors(Z_DIR, N));
    } else if (v.IsValid() && up.IsValid() && v != sm::vec3(0, 0, 0) && up != sm::vec3(0, 0, 0)) {
//        l_mt = sm::mat4::LookAt(sm::vec3(0, 0, 0), v, up).Inverted();
        l_mt = BuildRotMat(v, up);
    } else if (v.IsValid()) {
        l_mt = sm::mat4(sm::Quaternion::CreateFromVectors(Z_DIR, v));
    }

    // rot matrix
    auto rot_list = attr.QueryParmList(GeoAttrClass::Point, GEO_ATTR_ROT);
    if (rot_list && rot_list->GetType() == ParmType::Float4) {
        assert(rot_list->Size() > p_idx);
        auto& rot = std::static_pointer_cast<ParmFlt4List>(rot_list)->GetAllItems()[p_idx];
        r_mt = sm::mat4(sm::Quaternion(rot.x, rot.y, rot.z, rot.w));
    }

    // trans matrix (trans + P)
    auto& pos = attr.GetPoints()[p_idx]->pos;
    t_mt = sm::mat4::Translated(pos.x, pos.y, pos.z) * t_mt;
    auto trans_list = attr.QueryParmList(GeoAttrClass::Point, GEO_ATTR_TRANS);
    if (trans_list && trans_list->GetType() == ParmType::Vector) {
        assert(trans_list->Size() > p_idx);
        auto& trans = std::static_pointer_cast<ParmFlt3List>(trans_list)->GetAllItems()[p_idx];
        t_mt = sm::mat4::Translated(trans.x, trans.y, trans.z) * t_mt;
    }

    // transform matrix
    // todo

    if (trans_list) {
        mt = t_mt * m_mt * x_mt;
    } else if (orient_list) {
        mt = t_mt * (r_mt * o_mt) * s_mt * x_mt;
    } else {
        mt = t_mt * r_mt * l_mt * s_mt * x_mt;
    }

    return mt;
}

}
}