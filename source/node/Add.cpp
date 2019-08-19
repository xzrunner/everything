#include "everything/node/Add.h"
#include "everything/Geometry.h"

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
    auto& prev_points = prev_geo->GetAttr().GetPoints();
    vertices.reserve(prev_points.size());
    for (auto& p : prev_points) {
        vertices.push_back(p->pos);
    }
    if (vertices.empty() && m_points.empty()) {
        return;
    }

    m_geo = std::make_shared<Geometry>(GeoAdaptor::DataType::Shape);

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
    if (m_geo && !vertices.empty()) {
        auto shape = std::make_shared<gs::Polyline3D>(vertices);
        m_geo->UpdateByShape(shape);
    }
}

}
}