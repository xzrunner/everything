#pragma once

#include "everything/GeoAttribute.h"

namespace evt
{

class GeoAttrHelper
{
public:
    static sm::vec3 CalcFaceNormal(const GeoAttribute::Primitive& prim);

    static void GenAttrFromPrim(GeoAttribute& dst, const GeoAttribute& src, size_t prim_idx);

}; // GeoAttrHelper

}