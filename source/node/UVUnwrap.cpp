#include "sop/node/UVUnwrap.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/GeogramHelper.h"

#include <sm_const.h>

#include <geogram/mesh/mesh.h>
#include <geogram/parameterization/mesh_atlas_maker.h>

namespace sop
{
namespace node
{

void UVUnwrap::Execute(const ur::Device& dev, Evaluator& eval)
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

    GeogramHelper::BuildMesh(mesh, attr);

//    GEO::mesh_make_atlas(mesh, SM_PI * 0.5f, GEO::PARAM_LSCM, GEO::PACK_TETRIS, false);
    GEO::mesh_make_atlas_only_unwrap(mesh, SM_PI * 0.5f, GEO::PARAM_LSCM, false);

    GeogramHelper::LoadTexcoordsFromMesh(attr, mesh);
}

}
}