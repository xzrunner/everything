#include "everything/node/Curve.h"
#include "everything/GeometryNode.h"

#include <geoshape/Polyline3D.h>
#include <node0/SceneNode.h>
#include <node3/CompShape.h>
#include <node3/CompAABB.h>

namespace evt
{
namespace node
{

void Curve::Execute(TreeContext& ctx)
{
    m_geo = std::make_shared<GeometryNode>(GeometryNode::DataType::Shape);
    BuildModel();
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
    if (!m_geo) {
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

    auto node = m_geo->GetNode();
    assert(node);

    auto& cshape = node->GetSharedComp<n3::CompShape>();
    cshape.SetShape(shape);

    auto& caabb = node->GetUniqueComp<n3::CompAABB>();
    caabb.SetAABB(shape->GetBounding());
}

}
}