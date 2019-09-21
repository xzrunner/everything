#pragma once

#include "everything/VarValue.h"
#include "everything/GeoAttribute.h"

#include <map>
#include <vector>

namespace evt
{

class GeometryImpl;

class GeoAttrRebuild
{
public:
    GeoAttrRebuild(GeometryImpl& geo);
    ~GeoAttrRebuild();

private:
    void Save();
    void Load();

private:
    GeometryImpl& m_geo;

    std::map<int, std::vector<VarValue>> m_points;
    std::map<int, std::vector<VarValue>> m_vertices;
    std::map<int, std::vector<VarValue>> m_primitives;
    std::vector<VarValue>                m_detail;

    std::vector<GeoAttribute::VarDesc> m_var_descs[static_cast<int>(GeoAttrType::MaxTypeNum)];

}; // GeoAttrRebuild

}