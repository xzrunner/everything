#pragma once

#include "sop/GeoAttrType.h"

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
    "",
};

static const GeoAttrType GeoAttrTypes[] =
{
#define GEO_ATTR_INFO(attr, name, type) GeoAttrType::type,
#include "sop/geo_attr_cfg.h"
#undef GEO_ATTR_INFO
    GeoAttrType::Int,
};

}