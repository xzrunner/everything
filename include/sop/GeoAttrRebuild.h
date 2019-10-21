#pragma once

#include "sop/VarValue.h"
#include "sop/GeoAttribute.h"

#include <map>
#include <vector>

namespace sop
{

class GeometryImpl;

class GeoAttrRebuild
{
public:
    GeoAttrRebuild(GeometryImpl& geo);
    ~GeoAttrRebuild();

    static void Copy(GeometryImpl& dst, GeometryImpl& src);

private:
    struct Dump
    {
        std::map<uint64_t, std::vector<VarValue>> points;
        std::map<uint64_t, std::vector<VarValue>> vertices;
        std::map<uint64_t, std::vector<VarValue>> primitives;
        std::vector<VarValue>                     detail;

        std::vector<GeoAttribute::VarDesc> var_descs[static_cast<int>(GeoAttrClass::MaxTypeNum)];
    };

private:
    static void Save(Dump& dst, const GeometryImpl& src);
    static void Load(GeometryImpl& dst, const Dump& src);

private:
    GeometryImpl& m_geo;

    Dump m_dump;

}; // GeoAttrRebuild

}