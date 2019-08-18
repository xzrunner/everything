#pragma once

#include "everything/Variable.h"

#include <vector>

namespace evt
{

class Geometry
{
public:

private:
    struct Point
    {
        sm::vec3 pos;

        std::vector<std::unique_ptr<Variable>> vars;

    }; // Point

    struct Vertex
    {
        size_t primitive;
        size_t point;

        std::vector<std::unique_ptr<Variable>> vars;

    }; // Vertex

    struct Primitive
    {
        std::vector<std::unique_ptr<Variable>> vars;

    }; // Primitive

    struct Detail
    {
        std::vector<std::unique_ptr<Variable>> vars;

    }; // Detail

private:
    std::vector<Point>     m_points;
    std::vector<Vertex>    m_vertices;
    std::vector<Primitive> m_primtives;
    Detail                 m_detail;

}; // Geometry

}