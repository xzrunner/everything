#include "sop/GeogramHelper.h"
#include "sop/GeoAttribute.h"
#include "sop/ParmList.h"

#include <geogram/mesh/mesh.h>

namespace sop
{

void GeogramHelper::BuildMesh(GEO::Mesh& dst, const GeoAttribute& src)
{
    auto& pts = src.GetPoints();
    dst.vertices.create_vertices(pts.size());
    for (size_t i = 0, n = pts.size(); i < n; ++i)
    {
        double* p = dst.vertices.point_ptr(i);
        for (size_t c = 0; c < 3; ++c) {
            p[c] = pts[i]->pos.xyz[c];
        }
    }

    auto& prims = src.GetPrimtives();
    for (auto& prim : prims)
    {
        GEO::vector<GEO::index_t> vertices;
        vertices.reserve(prim->vertices.size());
        for (auto& v : prim->vertices) {
            vertices.push_back(v->point->attr_idx);
        }
        dst.facets.create_polygon(vertices);
    }
    dst.facets.connect();
}

void GeogramHelper::LoadTexcoordsFromMesh(GeoAttribute& dst, const GEO::Mesh& src)
{
    GEO::Attribute<double> tex_coord;
    tex_coord.bind_if_is_defined(
        src.facet_corners.attributes(), "tex_coord"
    );
    geo_assert(tex_coord.is_bound() && tex_coord.dimension() == 2);
    auto& prims = dst.GetPrimtives();
    assert(src.facets.nb() == prims.size());
    std::vector<sm::vec3> data(dst.GetVertices().size(), sm::vec3(0, 0, 0));
    for (size_t f = 0; f < src.facets.nb(); ++f)
    {
        auto& prim = prims[f];
        assert(src.facets.corners_end(f) - src.facets.corners_begin(f) == prim->vertices.size());
        size_t idx = 0;
        for (auto c = src.facets.corners_begin(f); c < src.facets.corners_end(f); ++c, ++idx)
        {
            float tx = static_cast<float>(tex_coord[2 * c]);
            float ty = static_cast<float>(tex_coord[2 * c + 1]);
            data[prim->vertices[idx]->attr_idx].Set(tx, ty, 0);
        }
    }
    dst.AddParmList(GeoAttrClass::Vertex,
        std::make_shared<ParmFlt3List>(GEO_ATTR_UV, data)
    );
}

void GeogramHelper::LoadTexcoordsToMesh(GEO::Mesh& dst, const GeoAttribute& src)
{
    GEO::Attribute<double> tex_coord;
    tex_coord.create_vector_attribute(
        dst.facet_corners.attributes(), "tex_coord", 2
    );
    auto uv_list = src.QueryParmList(GeoAttrClass::Vertex, GEO_ATTR_UV);
    assert(uv_list);
    size_t idx = 0;
    auto& prims = src.GetPrimtives();
    assert(uv_list->GetType() == ParmType::Float3);
    auto& uv_data = std::static_pointer_cast<ParmFlt3List>(uv_list)->GetAllItems();
    for (auto& prim : prims)
    {
        for (auto& v : prim->vertices)
        {
            assert(v->attr_idx < uv_data.size());
            auto& uv = uv_data[v->attr_idx];
            tex_coord[idx * 2]     = std::max(uv.x, 0.01f);
            tex_coord[idx * 2 + 1] = std::max(uv.y, 0.01f);
            ++idx;
        }
    }
}

}