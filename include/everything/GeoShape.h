#pragma once

#include <SM_Vector.h>

#include <vector>

namespace evt
{

enum class GeoShapeType
{
    Point,
    Polyline,
    Faces,
};

class GeoShape
{
public:
    virtual GeoShapeType Type() const = 0;

}; // GeoShape

class GeoPoint : public GeoShape
{
public:
    GeoPoint(const sm::vec3& vertex)
        : m_vertex(vertex)
    {
    }

    virtual GeoShapeType Type() const {
        return GeoShapeType::Point;
    }

    auto& GetVertex() const { return m_vertex; }

private:
    sm::vec3 m_vertex;

}; // GeoPoint

class GeoPolyline : public GeoShape
{
public:
    GeoPolyline(const std::vector<sm::vec3>& vertices)
        : m_vertices(vertices)
    {
    }

    virtual GeoShapeType Type() const {
        return GeoShapeType::Polyline;
    }

    auto& GetVertices() const { return m_vertices; }

private:
    std::vector<sm::vec3> m_vertices;

}; // GeoPolyline

class GeoFaces : public GeoShape
{
public:
    GeoFaces(const std::vector<std::vector<sm::vec3>>& vertices)
        : m_vertices(vertices)
    {
    }

    virtual GeoShapeType Type() const {
        return GeoShapeType::Faces;
    }

private:
    std::vector<std::vector<sm::vec3>> m_vertices;

}; // GeoFaces

}