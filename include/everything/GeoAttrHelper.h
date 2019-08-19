#pragma once

#include "everything/GeoAttribute.h"

namespace evt
{

class GeoAttrHelper
{
public:
    static sm::vec3 CalcFaceNormal(const GeoAttribute::Primitive& prim);

}; // GeoAttrHelper

}