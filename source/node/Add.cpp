#include "everything/node/Add.h"
#include "everything/GeometryNode.h"

#include <geoshape/Polyline3D.h>
#include <node0/SceneNode.h>
#include <node3/CompShape.h>
#include <node3/CompAABB.h>

namespace evt
{
namespace node
{

void Add::Execute(TreeContext& ctx)
{
    m_geo.reset();

    auto prev_geo = GetInputGeo(0);
    if (!prev_geo) {
        return;
    }

    std::vector<sm::vec3> vertices;
    prev_geo->TraversePoints([&](sm::vec3& p, bool& dirty)->bool
    {
        vertices.push_back(p);
        dirty = false;
        return true;
    });
    if (vertices.empty() && m_points.empty()) {
        return;
    }

    m_geo = std::make_shared<GeometryNode>(GeometryNode::DataType::Shape);

    std::copy(m_points.begin(), m_points.end(), std::back_inserter(vertices));
    BuildModel(vertices);
}

void Add::SetPoints(const std::vector<sm::vec3>& points)
{
    if (m_points == points) {
        return;
    }

    m_points = points;

    SetDirty(true);
}

void Add::BuildModel(const std::vector<sm::vec3>& vertices)
{
    if (!m_geo) {
        return;
    }
    if (vertices.empty()) {
        return;
    }

    auto shape = std::make_shared<gs::Polyline3D>(vertices);

    auto node = m_geo->GetNode();
    assert(node);

    auto& cshape = node->GetSharedComp<n3::CompShape>();
    cshape.SetShape(shape);

    auto& caabb = node->GetUniqueComp<n3::CompAABB>();
    caabb.SetAABB(shape->GetBounding());
}

}
}