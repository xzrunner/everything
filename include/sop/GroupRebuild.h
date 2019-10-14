#pragma once

#include "sop/GeoAttribute.h"

namespace sop
{

class Group;
class GeometryImpl;

class GroupRebuild
{
public:
    GroupRebuild(GeometryImpl& geo);
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
        std::vector<uint64_t> primtives;
    };

private:
    GeometryImpl& m_geo;

    std::vector<GroupDump> m_groups;

}; // GroupRebuild

}