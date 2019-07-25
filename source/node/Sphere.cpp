#include "everything/node/Sphere.h"

#include <model/ParametricEquations.h>
#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node0/CompMaterial.h>
#include <node3/CompMeshFilter.h>
#include <node3/CompTransform.h>
#include <node3/CompAABB.h>
#include <painting3/MaterialMgr.h>

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

        // CompMaterial
        auto& cmaterial = m_scene_node->AddUniqueComp<n0::CompMaterial>();
        auto mat = std::make_unique<pt0::Material>();
        typedef pt3::MaterialMgr::PhongUniforms UNIFORMS;
        mat->AddVar(UNIFORMS::ambient.name, pt0::RenderVariant(sm::vec3(0.04f, 0.04f, 0.04f)));
        mat->AddVar(UNIFORMS::diffuse.name, pt0::RenderVariant(sm::vec3(1, 1, 1)));
        mat->AddVar(UNIFORMS::specular.name, pt0::RenderVariant(sm::vec3(1, 1, 1)));
        mat->AddVar(UNIFORMS::shininess.name, pt0::RenderVariant(50.0f));
        cmaterial.SetMaterial(mat);
    }

    // CompTransform
    auto& ctrans = m_scene_node->GetUniqueComp<n3::CompTransform>();

    ctrans.SetPosition(m_sphere.GetCenter());

    const float s = m_sphere.GetRadius();
    ctrans.SetScale(sm::vec3(s, s, s));
}

}
}