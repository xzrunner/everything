#include "everything/node/Line.h"
#include "everything/Geometry.h"

#include <geoshape/Polyline3D.h>
#include <node0/SceneNode.h>
#include <node3/CompShape.h>
#include <node3/CompAABB.h>

namespace evt
{
namespace node
{

void Line::Execute(TreeContext& ctx)
{
    m_geo = std::make_shared<Geometry>(Geometry::DataType::Shape);
    BuildModel();
}

void Line::SetOrigin(const sm::vec3& ori)
{
    if (m_origin == ori) {
        return;
    }

    m_origin = ori;
    BuildModel();

    SetDirty(true);
}

void Line::SetDirection(const sm::vec3& dir)
{
    if (m_direction == dir) {
        return;
    }

    m_direction = dir;
    BuildModel();

    SetDirty(true);
}

void Line::SetLength(float len)
{
    if (m_length == len) {
        return;
    }

    m_length = len;
    BuildModel();

    SetDirty(true);
}

void Line::SetPoints(size_t num)
{
    if (m_points == num) {
        return;
    }

    m_points = num;
    BuildModel();

    SetDirty(true);
}

void Line::BuildModel()
{
    if (!m_geo) {
        return;
    }
    if (m_points < 2) {
        return;
    }

    std::vector<sm::vec3> vertices;
    vertices.reserve(m_points);
    auto dt = m_direction * m_length / static_cast<float>(m_points - 1);
    auto v = m_origin;
    for (size_t i = 0; i < m_points; ++i) {
        vertices.push_back(v);
        v += dt;
    }
    auto shape = std::make_shared<gs::Polyline3D>(vertices, false);

    auto node = m_geo->GetNode();
    assert(node);

    auto& cshape = node->GetSharedComp<n3::CompShape>();
    cshape.SetShape(shape);

    auto& caabb = node->GetUniqueComp<n3::CompAABB>();
    caabb.SetAABB(shape->GetBounding());
}

}
}
