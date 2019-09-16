#include "everything/node/Sphere.h"
#include "everything/GeometryImpl.h"

namespace evt
{
namespace node
{

void Sphere::Execute(Evaluator& eval, TreeContext& ctx)
{
    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Brush);
    BuildModel();
}

void Sphere::BuildModel()
{
    //auto& cmesh = m_scene_node->AddUniqueComp<n3::CompMeshFilter>();
    //cmesh.SetMesh(model::Sphere::TYPE_NAME);

    //m_scene_node->GetUniqueComp<n3::CompAABB>().SetAABB(cmesh.GetAABB());
}

}
}