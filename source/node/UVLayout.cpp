#include "sop/node/UVLayout.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/GeogramHelper.h"

#include <sm_const.h>

#include <geogram/parameterization/mesh_atlas_maker.h>
#include <geogram/mesh/mesh.h>

namespace sop
{
namespace node
{

void UVLayout::Execute(const ur2::Device& dev, Evaluator& eval)
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
    GeogramHelper::LoadTexcoordsToMesh(mesh, attr);

    GEO::mesh_make_atlas_only_layout(mesh, GEO::PACK_TETRIS);

    GeogramHelper::LoadTexcoordsFromMesh(attr, mesh);
}

}
}