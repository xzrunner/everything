#pragma once

#include "everything/Variable.h"

#include "SM_Cube.h"

#include <vector>

#include <boost/noncopyable.hpp>

namespace evt
{

class GeoShape;

class GeoAttribute : boost::noncopyable
{
public:
    struct Point
    {
        Point(const sm::vec3& pos) : pos(pos) {}

        sm::vec3 pos;

        size_t order = 0;

    }; // Point

    struct Primitive;
    struct Vertex
    {
        std::shared_ptr<Point>   point = nullptr;
        std::weak_ptr<Primitive> prim;

    }; // Vertex

    struct Primitive
    {
        std::vector<std::shared_ptr<Vertex>> vertices;

    }; // Primitive

    //struct Detail
    //{
    //}; // Detail

    struct AttrList
    {
        void Clear();

        std::vector<std::string> names;
        std::vector<Variable>    vars;

    }; // AttrList

public:
    GeoAttribute() {}
    GeoAttribute(const GeoShape& shape);
    GeoAttribute(const GeoAttribute& attr);
    GeoAttribute& operator = (const GeoAttribute& attr);

    void Clear();

    auto& GetPoints() { return m_points; }
    auto& GetPoints() const { return m_points; }
    void  SetPoints(const std::vector<std::shared_ptr<Point>>& points);

    auto& GetVertices() { return m_vertices; }
    auto& GetVertices() const { return m_vertices; }
    void  SetVertices(const std::vector<std::shared_ptr<Vertex>>& vertices);

    auto& GetPrimtives() { return m_primtives; }
    auto& GetPrimtives() const { return m_primtives; }
    void  SetPrimtives(const std::vector<std::shared_ptr<Primitive>>& prims);

    void Combine(const GeoAttribute& attr);

    void ResetPointsOrder();

    void FromGeoShape(const GeoShape& shape);

    auto& GetAABB() const { return m_aabb; }

private:
    void SetupAABB();

private:
    std::vector<std::shared_ptr<Point>>     m_points;
    std::vector<std::shared_ptr<Vertex>>    m_vertices;
    std::vector<std::shared_ptr<Primitive>> m_primtives;
//    Detail                 m_detail;

    AttrList m_attr_point;
    AttrList m_attr_vertex;
    AttrList m_attr_prim;
    AttrList m_attr_detail;

    sm::cube m_aabb;

    friend class GeoAdaptor;

}; // GeoAttribute

}