#include "sop/ParmList.h"

namespace sop
{

ParmList::ParmList(const ParmList& list)
{
    operator = (list);
}

ParmList& ParmList::operator = (const ParmList& list)
{
    m_attr = list.m_attr;
    m_name = list.m_name;
    m_type = list.m_type;

    return *this;
}

ParmList::ParmList(GeoAttr attr) 
    : m_attr(attr)
    , m_name(GeoAttrNames[attr])
    , m_type(GeoAttrTypes[attr])
{
}

ParmList::ParmList(const std::string& name, GeoAttrType type)
    : m_attr(GEO_ATTR_UNKNOWN)
    , m_name(name)
    , m_type(type)
{
    for (int i = 0; i < GEO_ATTR_UNKNOWN; ++i) {
        if (GeoAttrNames[i] == name) {
            m_attr = static_cast<GeoAttr>(i);
            break;
        }
    }
}

}