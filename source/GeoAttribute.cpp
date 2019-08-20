#include "everything/GeoAttribute.h"
#include "everything/GeoShape.h"

#include <map>

namespace evt
{

GeoAttribute::GeoAttribute(const GeoShape& shape)
{
    FromGeoShape(shape);
}

GeoAttribute::GeoAttribute(const GeoAttribute& attr)
{
    operator = (attr);
}

GeoAttribute& GeoAttribute::operator = (const GeoAttribute& attr)
{
    // prepare

    std::map<std::shared_ptr<Point>, size_t> point2idx;
    for (size_t i = 0, n = attr.m_points.size(); i < n; ++i) {
        point2idx.insert({ attr.m_points[i], i });
    }

    std::map<std::shared_ptr<Vertex>, size_t> vertex2idx;
    for (size_t i = 0, n = attr.m_vertices.size(); i < n; ++i) {
        vertex2idx.insert({ attr.m_vertices[i], i });
    }

    std::map<std::shared_ptr<Primitive>, size_t> prim2idx;
    for (size_t i = 0, n = attr.m_primtives.size(); i < n; ++i) {
        prim2idx.insert({ attr.m_primtives[i], i });
    }

    // points
    m_points.reserve(attr.m_points.size());
    for (auto& p : attr.m_points) {
        m_points.push_back(std::make_shared<Point>(*p));
    }

    // vertices
    m_vertices.reserve(attr.m_vertices.size());
    for (auto& v : attr.m_vertices) {
        m_vertices.push_back(std::make_shared<Vertex>());
    }

    // primitives
    m_primtives.reserve(attr.m_primtives.size());
    for (auto& prim : attr.m_primtives) {
        m_primtives.push_back(std::make_shared<Primitive>());
    }

    // connect
    for (size_t i = 0, n = attr.m_vertices.size(); i < n; ++i)
    {
        auto& src = attr.m_vertices[i];
        auto& dst = m_vertices[i];

        auto itr_p = point2idx.find(src->point);
        assert(itr_p != point2idx.end());
        auto itr_prim = prim2idx.find(src->prim.lock());
        assert(itr_prim != prim2idx.end());
        dst->point = m_points[itr_p->second];
        dst->prim = m_primtives[itr_prim->second];
    }
    for (size_t i = 0, n = attr.m_primtives.size(); i < n; ++i)
    {
        auto& src = attr.m_primtives[i];
        auto& dst = m_primtives[i];

        dst->vertices.reserve(src->vertices.size());
        for (auto& v : src->vertices)
        {
            auto itr = vertex2idx.find(v);
            assert(itr != vertex2idx.end());
            dst->vertices.push_back(m_vertices[itr->second]);
        }
    }

    // attr list
    m_attr_point  = attr.m_attr_point;
    m_attr_vertex = attr.m_attr_vertex;
    m_attr_prim   = attr.m_attr_prim;
    m_attr_detail = attr.m_attr_detail;

    return *this;
}

void GeoAttribute::Clear()
{
    m_points.clear();
    m_vertices.clear();
    m_primtives.clear();

    m_attr_point.Clear();
    m_attr_vertex.Clear();
    m_attr_prim.Clear();
    m_attr_detail.Clear();
}

void GeoAttribute::SetPoints(const std::vector<std::shared_ptr<Point>>& points)
{
    m_points = points;

    ResetPointsOrder();
}

void GeoAttribute::SetVertices(const std::vector<std::shared_ptr<Vertex>>& vertices)
{
    m_vertices = vertices;
}

void GeoAttribute::SetPrimtives(const std::vector<std::shared_ptr<Primitive>>& prims)
{
    m_primtives = prims;

    // shrink
    for (auto itr = m_vertices.begin(); itr != m_vertices.end(); )
    {
        if (itr->use_count() == 1) {
            itr = m_vertices.erase(itr);
        } else {
            ++itr;
        }
    }
    for (auto itr = m_points.begin(); itr != m_points.end(); )
    {
        if (itr->use_count() == 1) {
            itr = m_points.erase(itr);
        } else {
            ++itr;
        }
    }

    ResetPointsOrder();
}

void GeoAttribute::Combine(const GeoAttribute& attr)
{
    // todo attr_lsit
    std::copy(attr.m_points.begin(), attr.m_points.end(), std::back_inserter(m_points));
    std::copy(attr.m_vertices.begin(), attr.m_vertices.end(), std::back_inserter(m_vertices));
    std::copy(attr.m_primtives.begin(), attr.m_primtives.end(), std::back_inserter(m_primtives));
}

void GeoAttribute::ResetPointsOrder()
{
    for (size_t i = 0, n = m_points.size(); i < n; ++i) {
        m_points[i]->order = i;
    }
}

void GeoAttribute::FromGeoShape(const GeoShape& shape)
{
    Clear();

    switch (shape.Type())
    {
    case GeoShapeType::Points:
    {
        auto& vertices = static_cast<const GeoPoints&>(shape).GetVertices();
        m_points.reserve(vertices.size());
        for (auto& v : vertices) {
            m_points.push_back(std::make_shared<GeoAttribute::Point>(v));
        }
        ResetPointsOrder();
    }
        break;
    case GeoShapeType::Polyline:
    {
        auto& vertices = static_cast<const GeoPolyline&>(shape).GetVertices();
        if (vertices.size() < 2) {
            return;
        }
        auto from_polyline = [&](const std::vector<sm::vec3>& vertices)
        {
            m_points.reserve(vertices.size());
            auto prim = std::make_shared<GeoAttribute::Primitive>();
            prim->vertices.reserve(vertices.size());
            for (auto& v : vertices)
            {
                auto dst_p = std::make_shared<GeoAttribute::Point>(v);
                dst_p->order = m_points.size();
                m_points.push_back(dst_p);

                auto dst_v = std::make_shared<GeoAttribute::Vertex>();
                dst_v->point = dst_p;
                dst_v->prim = prim;
                m_vertices.push_back(dst_v);

                prim->vertices.push_back(dst_v);
            }
            m_primtives.push_back(prim);
        };
        if (vertices.front() == vertices.back()) {
            auto del_back = vertices;
            del_back.pop_back();
            from_polyline(del_back);
        } else {
            from_polyline(vertices);
        }
    }
        break;
    }
}

//////////////////////////////////////////////////////////////////////////
// struct GeoAttribute::AttrList
//////////////////////////////////////////////////////////////////////////
void GeoAttribute::AttrList::Clear()
{
    names.clear();
    vars.clear();
}

}