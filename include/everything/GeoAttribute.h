#pragma once

#include "everything/Variable.h"

#include <vector>

namespace evt
{

class GeoAttribute
{
public:
    void Clear();

    void Update();

    auto& GetPoints() { return m_points; }
    auto& GetPoints() const { return m_points; }

    auto& GetPrimtives() const { return m_primtives; }

private:
    void UpdatePointIndices();

public:
    struct Point
    {
        sm::vec3 pos;

        size_t point_idx = 0;

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

private:
    std::vector<std::shared_ptr<Point>>     m_points;
    std::vector<std::shared_ptr<Vertex>>    m_vertices;
    std::vector<std::shared_ptr<Primitive>> m_primtives;
//    Detail                 m_detail;

    AttrList m_attr_point;
    AttrList m_attr_vertex;
    AttrList m_attr_prim;
    AttrList m_attr_detail;

    friend class GeoAdaptor;

}; // GeoAttribute

}