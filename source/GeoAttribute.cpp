#include "everything/GeoAttribute.h"

namespace evt
{

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

void GeoAttribute::Update()
{
    UpdatePointIndices();
}

void GeoAttribute::UpdatePointIndices()
{
    for (size_t i = 0, n = m_points.size(); i < n; ++i) {
        m_points[i]->point_idx = i;
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