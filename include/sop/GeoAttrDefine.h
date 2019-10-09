#pragma once

#include "sop/GeoAttrVarType.h"

namespace sop
{

enum GeoAttr
{
#define GEO_ATTR_INFO(attr, name, type) attr,
#include "sop/geo_attr_cfg.h"
#undef GEO_ATTR_INFO
    GEO_ATTR_UNKNOWN,
};

static const char* GeoAttrNames[] =
{
#define GEO_ATTR_INFO(attr, name, type) name,
#include "sop/geo_attr_cfg.h"
#undef GEO_ATTR_INFO
    nullptr,
};

static const GeoAttrVarType GeoAttrTypes[] =
{
#define GEO_ATTR_INFO(attr, name, type) GeoAttrVarType::type,
#include "sop/geo_attr_cfg.h"
#undef GEO_ATTR_INFO
    GeoAttrVarType::Int,
};

}