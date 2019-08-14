#include "everything/node/Curve.h"

#include <geoshape/Polyline3D.h>
#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node3/CompShape.h>
#include <node3/CompAABB.h>

namespace evt
{
namespace node
{

void Curve::Execute(TreeContext& ctx)
{
    if (!m_scene_node)
    {
        m_scene_node = ns::NodeFactory::Create3D();

        m_scene_node->AddSharedComp<n3::CompShape>();

        BuildModel();
    }
}

void Curve::SetVertices(const std::vector<sm::vec3>& vertices)
{
    if (m_vertices == vertices) {
        return;
    }

    m_vertices = vertices;
    BuildModel();

    SetDirty(true);
}

void Curve::BuildModel()
{
    if (!m_scene_node) {
        return;
    }

    if (m_vertices.size() < 2) {
        return;
    }

    std::shared_ptr<gs::Shape3D> shape = nullptr;
    if (m_vertices.front() == m_vertices.back()) {
        auto vertices(m_vertices);
        vertices.pop_back();
        shape = std::make_shared<gs::Polyline3D>(vertices, true);
    } else {
        shape = std::make_shared<gs::Polyline3D>(m_vertices, false);
    }

    auto& cshape = m_scene_node->GetSharedComp<n3::CompShape>();
    cshape.SetShape(shape);

    auto& caabb = m_scene_node->GetUniqueComp<n3::CompAABB>();
    caabb.SetAABB(shape->GetBounding());
}

}
}