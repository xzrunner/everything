#pragma once

#include "sop/GeoAttrType.h"
#include "sop/VarValue.h"

#include <SM_Vector.h>

#include <vector>

namespace sop
{

class GeoAttribute;

class GeoAttrHelper
{
public:
    static bool QueryNormalIndices(const GeoAttribute& attr, GeoAttrType type, sm::ivec3& norm_idx);

    static bool GetNormal(const GeoAttribute& attr, GeoAttrType type, size_t item_idx,
        const sm::ivec3& norm_idx, sm::vec3& norm);
    static bool SetNormal(const GeoAttribute& attr, GeoAttrType type, size_t item_idx,
        const sm::ivec3& norm_idx, const sm::vec3& norm);

    static bool GetNormal(const GeoAttribute& attr, const std::vector<VarValue>& vars,
        const sm::ivec3& norm_idx, sm::vec3& norm);
    static bool SetNormal(const GeoAttribute& attr, std::vector<VarValue>& vars,
        const sm::ivec3& norm_idx, const sm::vec3& norm);

}; // GeoAttrHelper

}
