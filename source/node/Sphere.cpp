#include "sop/node/Sphere.h"
#include "sop/GeometryImpl.h"

namespace sop
{
namespace node
{

void Sphere::Execute(const ur::Device& dev, Evaluator& eval)
{
    m_geo_impl = std::make_shared<GeometryImpl>(dev, GeoAdaptor::Type::Brush);
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