#include "sop/node/UVUnwrap.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

#include <geogram/parameterization/mesh_atlas_maker.h>
#include <geogram/mesh/mesh.h>

namespace sop
{
namespace node
{

void UVUnwrap::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    auto& attr = m_geo_impl->GetAttr();

    GEO::initialize();

    GEO::Mesh mesh(3);

    auto& pts = attr.GetPoints();
    mesh.vertices.create_vertices(pts.size());
    for (size_t i = 0, n = pts.size(); i < n; ++i)
    {
        double* p = mesh.vertices.point_ptr(i);
        for (size_t c = 0; c < 3; ++c) {
            p[c] = pts[i]->pos.xyz[c];
        }
    }

    auto& prims = attr.GetPrimtives();
    for (auto& prim : prims)
    {
        GEO::vector<GEO::index_t> vertices;
        vertices.reserve(prim->vertices.size());
        for (auto& v : prim->vertices) {
            vertices.push_back(v->point->attr_idx);
        }
        mesh.facets.create_polygon(vertices);
    }

    GEO::mesh_make_atlas(mesh);

    GEO::Attribute<double> tex_coord;
    tex_coord.bind_if_is_defined(
        mesh.facet_corners.attributes(), "tex_coord"
    );
    geo_assert(tex_coord.is_bound() && tex_coord.dimension() == 2);
    assert(mesh.facets.nb() == prims.size());
    std::vector<VarValue> vars(attr.GetVertices().size(), VarValue(sm::vec3(0 ,0, 0)));
    for (size_t f = 0; f < mesh.facets.nb(); ++f)
    {
        auto& prim = prims[f];
        assert(mesh.facets.corners_end(f) - mesh.facets.corners_begin(f) == prim->vertices.size());
        size_t idx = 0;
        for (auto c = mesh.facets.corners_begin(f); c < mesh.facets.corners_end(f); ++c, ++idx)
        {
            float tx = tex_coord[2 * c];
            float ty = tex_coord[2 * c + 1];
            vars[prim->vertices[idx]->attr_idx] = VarValue(sm::vec3(tx, ty, 0));
        }
    }
    attr.AddAttr(GeoAttrClass::Vertex, GEO_ATTR_UV, vars);
}

}
}