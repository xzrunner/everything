#include "everything/GeoAttrHelper.h"

#include <SM_Plane.h>

#include <map>

namespace evt
{

sm::vec3 GeoAttrHelper::CalcFaceNormal(const GeoAttribute::Primitive& prim)
{
    assert(prim.vertices.size() > 2);
    sm::Plane plane;
    plane.Build(
        prim.vertices[0]->point->pos,
        prim.vertices[1]->point->pos,
        prim.vertices[2]->point->pos
    );
    return plane.normal;
}

void GeoAttrHelper::GenAttrFromPrim(GeoAttribute& dst, const GeoAttribute& src, size_t prim_idx)
{
    auto& src_prims = src.GetPrimtives();
    if (prim_idx < 0 || prim_idx >= src_prims.size()) {
        return;
    }

    auto& src_prim = src_prims[prim_idx];
    auto& src_pts = src.GetPoints();
    auto& src_vts = src.GetVertices();

    std::vector<std::shared_ptr<GeoAttribute::Point>> dst_pts;
    std::vector<std::shared_ptr<GeoAttribute::Vertex>> dst_vts;

    std::map<std::shared_ptr<GeoAttribute::Point>, size_t> s_pt_to_d_idx;

    auto& dst_prim = std::make_shared<GeoAttribute::Primitive>();
    dst_prim->vertices.reserve(src_prim->vertices.size());
    for (auto& src_v : src_prim->vertices)
    {
        auto dst_v = std::make_shared<GeoAttribute::Vertex>();
        dst_v->prim = dst_prim;

        auto itr = s_pt_to_d_idx.find(src_v->point);
        if (itr == s_pt_to_d_idx.end())
        {
            auto& src_p = src_v->point;
            auto dst_p = std::make_shared<GeoAttribute::Point>(src_p->pos);
            s_pt_to_d_idx.insert({ src_p, dst_pts.size() });
            dst_pts.push_back(dst_p);

            dst_v->point = dst_p;
        }
        else
        {
            dst_v->point = dst_pts[itr->second];
        }

        dst_prim->vertices.push_back(dst_v);

        dst_vts.push_back(dst_v);
    }

    dst.Reset(dst_pts, dst_vts, { dst_prim });
}

}