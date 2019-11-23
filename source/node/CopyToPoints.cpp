#include "sop/node/CopyToPoints.h"
#include "sop/node/Normal.h"

#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

#include <SM_Quaternion.h>

namespace
{

const sm::vec3 Z_DIR(0, 0, 1);

int QueryAttrIdx(const std::vector<sop::GeoAttribute::VarDesc>& desc, sop::GeoAttr attr)
{
    for (int i = 0, n = desc.size(); i < n; ++i) {
        if (desc[i].GetName() == sop::GeoAttrNames[attr]) {
            return i;
        }
    }
    return -1;
}

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

void CopyToPoints::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto src_geo = NodeHelper::GetInputGeo(*this, IDX_SRC_PRIM);
    auto dst_geo = NodeHelper::GetInputGeo(*this, IDX_TARGET_POS);
    if (!src_geo || !dst_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Brush);

    if (!m_trans_use_pt_dir)
    {
        CopyTo(*src_geo, *dst_geo);
    }
    else
    {
        int norm_idx = dst_geo->GetAttr().QueryAttrIdx(GeoAttrClass::Point, GEO_ATTR_NORM);
        if (norm_idx < 0)
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
            CopyTo(*src_geo, *dst_geo, norm_idx);
        }
    }

    m_geo_impl->UpdateByAttr();
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
            CopyTo(src, *pts[i], dst.GetAttr().GetAttrDesc(GeoAttrClass::Point));
        }
    } else {
        for (auto& p : pts) {
            CopyTo(src, *p, dst.GetAttr().GetAttrDesc(GeoAttrClass::Point));
        }
    }
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeometryImpl& dst, size_t norm_idx)
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
            CopyTo(src, *pts[i], dst.GetAttr().GetAttrDesc(GeoAttrClass::Point),
                *static_cast<const sm::vec3*>(pts[i]->vars[norm_idx].p));
        }
    } else {
        for (auto& p : pts) {
            CopyTo(src, *p, dst.GetAttr().GetAttrDesc(GeoAttrClass::Point),
                *static_cast<const sm::vec3*>(p->vars[norm_idx].p));
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
            CopyTo(src, *pts[i], dst.GetAttr().GetAttrDesc(GeoAttrClass::Point), norms[i]);
        }
    } else {
        for (size_t i = 0, n = pts.size(); i < n; ++i) {
            CopyTo(src, *pts[i], dst.GetAttr().GetAttrDesc(GeoAttrClass::Point), norms[i]);
        }
    }
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeoAttribute::Point& dst,
                          const std::vector<GeoAttribute::VarDesc>& desc)
{
    auto& attr = m_geo_impl->GetAttr();

    m_geo_impl->GetGroup().Combine(src.GetGroup(), attr.GetPoints().size(),
        attr.GetVertices().size(), attr.GetPrimtives().size());

    sm::vec3 norm;
    norm.MakeInvalid();
    auto mt = CalcMat(dst, desc, norm);

    GeoAttribute src_attr(src.GetAttr());
    for (auto& p : src_attr.GetPoints()) {
        p->pos = mt * p->pos;
    }
    attr.Combine(src_attr);
}

void CopyToPoints::CopyTo(const GeometryImpl& src, const GeoAttribute::Point& dst,
                          const std::vector<GeoAttribute::VarDesc>& desc, const sm::vec3& norm)
{
    auto& attr = m_geo_impl->GetAttr();

    m_geo_impl->GetGroup().Combine(src.GetGroup(), attr.GetPoints().size(),
        attr.GetVertices().size(), attr.GetPrimtives().size());

    auto mt = CalcMat(dst, desc, norm);

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

sm::mat4 CopyToPoints::CalcMat(const GeoAttribute::Point& pt, const std::vector<GeoAttribute::VarDesc>& desc,
                               const sm::vec3& norm) const
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
    int pivot_idx = QueryAttrIdx(desc, GEO_ATTR_PIVOT);
    if (pivot_idx >= 0 && desc[pivot_idx].GetType() == GeoAttrType::Vector) {
        auto pivot = *static_cast<const sm::vec3*>(pt.vars[pivot_idx].p);
        x_mt = sm::mat4::Translated(-pivot.x, -pivot.y, -pivot.z);
    }

    // orient matrix
    int orient_idx = QueryAttrIdx(desc, GEO_ATTR_ORIENT);
    if (orient_idx >= 0 && desc[orient_idx].GetType() == GeoAttrType::Float4) {
        auto orient = *static_cast<const sm::vec4*>(pt.vars[orient_idx].p);
        o_mt = sm::mat4(sm::Quaternion(orient.x, orient.y, orient.z, orient.w));
    }

    // scale matrix (scale * pscale)
    int pscale_idx = QueryAttrIdx(desc, GEO_ATTR_PSCALE);
    if (pscale_idx >= 0 && desc[pscale_idx].GetType() == GeoAttrType::Float) {
        auto pscale = pt.vars[pscale_idx].f;
        s_mt = sm::mat4::Scaled(pscale, pscale, pscale) * s_mt;
    }
    int scale_idx = QueryAttrIdx(desc, GEO_ATTR_SCALE);
    if (scale_idx >= 0 && desc[scale_idx].GetType() == GeoAttrType::Vector) {
        auto scale = *static_cast<const sm::vec3*>(pt.vars[scale_idx].p);
        s_mt = sm::mat4::Scaled(scale.x, scale.y, scale.z) * s_mt;
    }

    // alignment matrix, defined by N or v and up.
    sm::vec3 N, v, up;
    int norm_idx = QueryAttrIdx(desc, GEO_ATTR_NORM);
    if (norm_idx >= 0 && desc[norm_idx].GetType() == GeoAttrType::Vector) {
        N = *static_cast<const sm::vec3*>(pt.vars[norm_idx].p);
    } else {
        N = norm;
    }
    int spd_idx = QueryAttrIdx(desc, GEO_ATTR_SPD);
    if (spd_idx >= 0 && desc[spd_idx].GetType() == GeoAttrType::Vector) {
        v = *static_cast<const sm::vec3*>(pt.vars[spd_idx].p);
    } else {
        v.MakeInvalid();
    }
    int up_idx = QueryAttrIdx(desc, GEO_ATTR_UP);
    if (up_idx >= 0 && desc[up_idx].GetType() == GeoAttrType::Vector) {
        up = *static_cast<const sm::vec3*>(pt.vars[up_idx].p);
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
    int rot_idx = QueryAttrIdx(desc, GEO_ATTR_ROT);
    if (rot_idx >= 0 && desc[rot_idx].GetType() == GeoAttrType::Float4) {
        auto rot = *static_cast<const sm::vec4*>(pt.vars[rot_idx].p);
        r_mt = sm::mat4(sm::Quaternion(rot.x, rot.y, rot.z, rot.w));
    }

    // trans matrix (trans + P)
    t_mt = sm::mat4::Translated(pt.pos.x, pt.pos.y, pt.pos.z) * t_mt;
    int trans_idx = QueryAttrIdx(desc, GEO_ATTR_TRANS);
    if (trans_idx >= 0 && desc[trans_idx].GetType() == GeoAttrType::Float) {
        auto trans = *static_cast<const sm::vec4*>(pt.vars[trans_idx].p);
        t_mt = sm::mat4::Translated(trans.x, trans.y, trans.z) * t_mt;
    }

    // transform matrix
    // todo

    if (trans_idx >= 0) {
        mt = t_mt * m_mt * x_mt;
    } else if (orient_idx >= 0) {
        mt = t_mt * (r_mt * o_mt) * s_mt * x_mt;
    } else {
        mt = t_mt * r_mt * l_mt * s_mt * x_mt;
    }

    return mt;
}

}
}