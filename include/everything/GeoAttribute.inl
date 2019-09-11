#pragma once

namespace evt
{

template<>
inline int GeoAttribute::QueryIndex(const std::shared_ptr<Point>& p) const
{
    for (int i = 0, n = m_points.size(); i < n; ++i) {
        if (m_points[i] == p) {
            return i;
        }
    }
    return -1;
}

template<>
inline int GeoAttribute::QueryIndex(const std::shared_ptr<Vertex>& v) const
{
    for (int i = 0, n = m_vertices.size(); i < n; ++i) {
        if (m_vertices[i] == v) {
            return i;
        }
    }
    return -1;
}

template<>
inline int GeoAttribute::QueryIndex(const std::shared_ptr<Primitive>& prim) const
{
    for (int i = 0, n = m_primtives.size(); i < n; ++i) {
        if (m_primtives[i] == prim) {
            return i;
        }
    }
    return -1;
}

}