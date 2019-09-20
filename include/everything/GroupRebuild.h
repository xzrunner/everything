#pragma once

#include "everything/GeoAttribute.h"

namespace evt
{

struct Group;

class GroupRebuild
{
public:
    GroupRebuild(const GeoAttribute& attr, Group& group);
    ~GroupRebuild();

private:
    const GeoAttribute& m_attr;
    Group& m_group;

    std::vector<std::shared_ptr<GeoAttribute::Point>>     m_points;
    std::vector<std::shared_ptr<GeoAttribute::Vertex>>    m_vertices;
    std::vector<std::shared_ptr<GeoAttribute::Primitive>> m_primtives;

}; // GroupRebuild

}