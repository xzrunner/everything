#pragma once

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
        static const size_t MAX_LSIT_COUNT = static_cast<size_t>(GeoAttrClass::MaxTypeNum);
        std::vector<std::shared_ptr<ParmList>> parm_lists[MAX_LSIT_COUNT];

        std::map<uint64_t, uint32_t> points;
        std::map<uint64_t, uint32_t> vertices;
        std::map<uint64_t, uint32_t> primitives;
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