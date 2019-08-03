#include "everything/node/Sphere.h"
#include "everything/NodeHelper.h"

#include <model/ParametricEquations.h>
#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node3/CompTransform.h>
#include <node3/CompAABB.h>
#include <node3/CompMeshFilter.h>

namespace evt
{
namespace node
{

void Sphere::ExecuteSelf()
{
    if (!m_scene_node)
    {
        m_scene_node = ns::NodeFactory::Create3D();

        auto& cmesh = m_scene_node->AddUniqueComp<n3::CompMeshFilter>();
        cmesh.SetMesh(model::Sphere::TYPE_NAME);

        m_scene_node->GetUniqueComp<n3::CompAABB>().SetAABB(cmesh.GetAABB());

        NodeHelper::AddMaterialComp(*m_scene_node);
    }

    // CompTransform
    auto& ctrans = m_scene_node->GetUniqueComp<n3::CompTransform>();

    ctrans.SetPosition(m_sphere.GetCenter());

    const float s = m_sphere.GetRadius();
    ctrans.SetScale(sm::vec3(s, s, s));
}

}
}