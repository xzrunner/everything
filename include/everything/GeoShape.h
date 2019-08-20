#pragma once

#include <SM_Vector.h>

#include <vector>

namespace evt
{

enum class GeoShapeType
{
    Points,
    Polyline,
    Faces,
};

class GeoShape
{
public:
    virtual GeoShapeType Type() const = 0;

}; // GeoShape

class GeoPoints : public GeoShape
{
public:
    GeoPoints(const std::vector<sm::vec3>& vertices)
        : m_vertices(vertices)
    {
    }

    virtual GeoShapeType Type() const { 
        return GeoShapeType::Points; 
    }

    auto& GetVertices() const { return m_vertices; }

private:
    std::vector<sm::vec3> m_vertices;

}; // GeoPoints

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