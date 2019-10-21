#include "sop/node/Add.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/GeoAttrRebuild.h"

#include <halfedge/Polyline.h>

namespace sop
{
namespace node
{

void Add::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto vertices = GetPrevVertices();
    if (vertices.empty() && m_points.empty()) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Shape);

    for (auto& p : m_points) {
        vertices.push_back({ he::TopoID(), p });
    }

    std::vector<std::pair<he::TopoID, sm::vec3>> line_vertices;
    std::vector<std::pair<he::TopoID, std::vector<size_t>>> line_polylines;
    line_vertices.reserve(vertices.size());
    for (auto& v : vertices) {
        line_vertices.push_back(v);
    }
    std::vector<size_t> indices(vertices.size());
    for (size_t i = 0, n = vertices.size(); i < n; ++i) {
        indices[i] = i;
    }
    line_polylines.push_back({ he::TopoID(), { indices } });
    auto line = std::make_shared<he::Polyline>(line_vertices, line_polylines);
    m_geo_impl->SetTopoLines({ line });

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (prev_geo) {
        GeoAttrRebuild::Copy(*m_geo_impl, *prev_geo);
    }
}

void Add::SetPoints(const std::vector<sm::vec3>& points)
{
    NODE_PROP_SET(m_points, points);
}

void Add::SetGroupName(const std::string& name)
{
    NODE_PROP_SET(m_group_name, name);
}

void Add::SetGroupType(GroupType type)
{
    NODE_PROP_SET(m_group_type, type);
}

std::vector<std::pair<he::TopoID, sm::vec3>>
Add::GetPrevVertices() const
{
    std::vector<std::pair<he::TopoID, sm::vec3>> vertices;

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return vertices;
    }

    std::shared_ptr<Group> group = nullptr;
    if (!m_group_name.empty())
    {
        group = prev_geo->GetGroup().Query(m_group_name);
        if (!group) {
            return vertices;
        }
    }

    auto& prev_points = prev_geo->GetAttr().GetPoints();
    if (group)
    {
        auto type = m_group_type == GroupType::GuessFromGroup ? group->GetType() : m_group_type;
        if (type != group->GetType()) {
            return vertices;
        }

        switch (type)
        {
        case GroupType::Points:
        {
            vertices.reserve(group->GetItems().size());
            for (auto& i : group->GetItems()) {
                auto& p = prev_points[i];
                vertices.push_back({ p->topo_id, p->pos });
            }
        }
            break;
        }
    }
    else
    {
        vertices.reserve(prev_points.size());
        for (auto& p : prev_points) {
            vertices.push_back({ p->topo_id, p->pos });
        }
    }

    return vertices;
}

}
}