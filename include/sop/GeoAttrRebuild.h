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
    GeoAttrRebuild(GeometryImpl& geo, bool prim_split = false);
    ~GeoAttrRebuild();

    static void Copy(GeometryImpl& dst, GeometryImpl& src, bool prim_split = false);

private:
    struct AttrDump
    {
        std::map<uint64_t, std::vector<VarValue>> points;
        std::map<uint64_t, std::vector<VarValue>> vertices;
        std::map<uint64_t, std::vector<VarValue>> primitives;
        std::vector<VarValue>                     detail;

        std::vector<GeoAttribute::VarDesc> var_descs[static_cast<int>(GeoAttrClass::MaxTypeNum)];
    };

private:
    static void Save(AttrDump& dst, const GeometryImpl& src);
    static void Load(GeometryImpl& dst, const AttrDump& src, bool prim_split);

private:
    GeometryImpl& m_geo;

    bool m_prim_split;

    AttrDump m_dump;

}; // GeoAttrRebuild

}