#include "everything/node/Sphere.h"
#include "everything/Geometry.h"

namespace evt
{
namespace node
{

void Sphere::Execute(TreeContext& ctx)
{
    m_geo = std::make_shared<Geometry>(GeoAdaptor::DataType::Brush);
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