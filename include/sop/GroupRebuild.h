#pragma once

#include "sop/GeoAttribute.h"

#include <set>

namespace sop
{

class Group;
class GeometryImpl;

class GroupRebuild
{
public:
    GroupRebuild(GeometryImpl& geo, bool prim_split = false);
    ~GroupRebuild();

private:
    void Save();
    void Load();

private:
    struct GroupDump
    {
        const Group* ori = nullptr;

        std::vector<uint64_t> points;
        std::vector<uint64_t> vertices;
        std::set<uint32_t> primtives;
    };

private:
    GeometryImpl& m_geo;

    bool m_prim_split;

    std::vector<GroupDump> m_groups;

}; // GroupRebuild

}