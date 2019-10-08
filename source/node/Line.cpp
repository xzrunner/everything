#include "sop/node/Line.h"
#include "sop/GeometryImpl.h"
#include "sop/GeoAdaptor.h"

#include <halfedge/Polyline.h>

namespace sop
{
namespace node
{

void Line::Execute(Evaluator& eval)
{
    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Shape);
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
    if (m_props.SetValue(LENGTH, Variable(len))) {
        SetDirty(true);
    }
}

void Line::SetPoints(size_t num)
{
    if (m_points == num) {
        return;
    }

    m_points = num;

    SetDirty(true);
}

void Line::InitProps()
{
    m_props.Assign(LENGTH, PropNames[LENGTH], Variable(1.0f));
}

void Line::BuildModel()
{
    if (!m_geo_impl) {
        return;
    }
    if (m_points < 2) {
        return;
    }

    std::vector<std::pair<he::TopoID, sm::vec3>> line_vertices;
    std::vector<std::pair<he::TopoID, std::vector<size_t>>> line_polylines;
    line_vertices.reserve(m_points);
    std::vector<size_t> indices(m_points);

    auto& props = m_props.GetProps();
    assert(props[LENGTH].Val().type == VarType::Float);
    const float length = props[LENGTH].Val().f;
    auto dt = m_direction * length / static_cast<float>(m_points - 1);
    auto v = m_origin;
    for (size_t i = 0; i < m_points; ++i)
    {
        line_vertices.push_back({ he::TopoID(), v });
        v += dt;
        indices[i] = i;
    }
    line_polylines.push_back({ he::TopoID(), { indices } });

    auto line = std::make_shared<he::Polyline>(line_vertices, line_polylines);
    m_geo_impl->SetTopoLines({ line });
}

}
}