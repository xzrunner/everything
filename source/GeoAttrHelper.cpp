#include "everything/GeoAttrHelper.h"
#include "everything/GeoAttribute.h"
#include "everything/GeoAttrName.h"

namespace evt
{

bool GeoAttrHelper::QueryNormalIndices(const GeoAttribute& attr, GeoAttrType type, sm::ivec3& norm_idx)
{
    norm_idx.Set(-1, -1, -1);

    auto& desc = attr.GetAttrDesc(type);
    for (int i = 0, n = desc.size(); i < n; ++i)
    {
        if (desc[i].name == GeoAttrName::norm_x) {
            assert(desc[i].type == VarType::Float);
            norm_idx.x = i;
        }
        if (desc[i].name == GeoAttrName::norm_y) {
            assert(desc[i].type == VarType::Float);
            norm_idx.y = i;
        }
        if (desc[i].name == GeoAttrName::norm_z) {
            assert(desc[i].type == VarType::Float);
            norm_idx.z = i;
        }
    }

    return norm_idx.x >= 0 && norm_idx.y >= 0 && norm_idx.z >= 0;
}

bool GeoAttrHelper::GetNormal(const GeoAttribute& attr, GeoAttrType type, size_t item_idx,
                              const sm::ivec3& norm_idx, sm::vec3& norm)
{
    if (norm_idx.x < 0 || norm_idx.y < 0 || norm_idx.z < 0) {
        return false;
    }

    auto& desc = attr.GetAttrDesc(type);
    assert(desc[norm_idx.x].name == GeoAttrName::norm_x 
        && desc[norm_idx.y].name == GeoAttrName::norm_y
        && desc[norm_idx.z].name == GeoAttrName::norm_z);
    assert(desc[norm_idx.x].type == VarType::Float 
        && desc[norm_idx.y].type == VarType::Float
        && desc[norm_idx.z].type == VarType::Float);
    switch (type)
    {
    case GeoAttrType::Point:
    {
        auto& pts = attr.GetPoints();
        assert(item_idx < pts.size());
        if (item_idx >= pts.size()) {
            return false;
        }
        auto& p = pts[item_idx];
        norm.x = p->vars[norm_idx.x].f;
        norm.y = p->vars[norm_idx.y].f;
        norm.z = p->vars[norm_idx.z].f;
    }
        break;
    case GeoAttrType::Vertex:
    {
        auto& vts = attr.GetVertices();
        assert(item_idx < vts.size());
        if (item_idx >= vts.size()) {
            return false;
        }
        auto& v = vts[item_idx];
        norm.x = v->vars[norm_idx.x].f;
        norm.y = v->vars[norm_idx.y].f;
        norm.z = v->vars[norm_idx.z].f;
    }
        break;
    case GeoAttrType::Primitive:
    {
        auto& prims = attr.GetPrimtives();
        assert(item_idx < prims.size());
        if (item_idx >= prims.size()) {
            return false;
        }
        auto& prim = prims[item_idx];
        norm.x = prim->vars[norm_idx.x].f;
        norm.y = prim->vars[norm_idx.y].f;
        norm.z = prim->vars[norm_idx.z].f;
    }
        break;
    case GeoAttrType::Detail:
    {
        auto& detail = attr.GetDetail();
        norm.x = detail.vars[norm_idx.x].f;
        norm.y = detail.vars[norm_idx.y].f;
        norm.z = detail.vars[norm_idx.z].f;
    }
        break;
    default:
        assert(0);
        return false;
    }

    return true;
}

bool GeoAttrHelper::SetNormal(const GeoAttribute& attr, GeoAttrType type, size_t item_idx,
                              const sm::ivec3& norm_idx, const sm::vec3& norm)
{
    if (norm_idx.x < 0 || norm_idx.y < 0 || norm_idx.z < 0) {
        return false;
    }

    auto& desc = attr.GetAttrDesc(type);
    assert(desc[norm_idx.x].name == GeoAttrName::norm_x 
        && desc[norm_idx.y].name == GeoAttrName::norm_y
        && desc[norm_idx.z].name == GeoAttrName::norm_z);
    assert(desc[norm_idx.x].type == VarType::Float 
        && desc[norm_idx.y].type == VarType::Float
        && desc[norm_idx.z].type == VarType::Float);
    switch (type)
    {
    case GeoAttrType::Point:
    {
        auto& pts = attr.GetPoints();
        assert(item_idx < pts.size());
        if (item_idx >= pts.size()) {
            return false;
        }
        auto p = std::static_pointer_cast<GeoAttribute::Point>(pts[item_idx]);
        p->vars[norm_idx.x].f = norm.x;
        p->vars[norm_idx.y].f = norm.y;
        p->vars[norm_idx.z].f = norm.z;
    }
        break;
    case GeoAttrType::Vertex:
    {
        auto& vts = attr.GetVertices();
        assert(item_idx < vts.size());
        if (item_idx >= vts.size()) {
            return false;
        }
        auto v = std::static_pointer_cast<GeoAttribute::Vertex>(vts[item_idx]);
        v->vars[norm_idx.x].f = norm.x;
        v->vars[norm_idx.y].f = norm.y;
        v->vars[norm_idx.z].f = norm.z;
    }
        break;
    case GeoAttrType::Primitive:
    {
        auto& prims = attr.GetPrimtives();
        assert(item_idx < prims.size());
        if (item_idx >= prims.size()) {
            return false;
        }
        auto prim = std::static_pointer_cast<GeoAttribute::Primitive>(prims[item_idx]);
        prim->vars[norm_idx.x].f = norm.x;
        prim->vars[norm_idx.y].f = norm.y;
        prim->vars[norm_idx.z].f = norm.z;
    }
        break;
    case GeoAttrType::Detail:
    {
        auto& detail = const_cast<GeoAttribute::Detail&>(attr.GetDetail());
        detail.vars[norm_idx.x].f = norm.x;
        detail.vars[norm_idx.y].f = norm.y;
        detail.vars[norm_idx.z].f = norm.z;
    }
        break;
    default:
        assert(0);
        return false;
    }

    return true;
}

bool GeoAttrHelper::GetNormal(const GeoAttribute& attr, const std::vector<VarValue>& vars,
                              const sm::ivec3& norm_idx, sm::vec3& norm)
{
    if (norm_idx.x < 0 || 
        norm_idx.y < 0 || 
        norm_idx.z < 0 ||
        norm_idx.x >= static_cast<int>(vars.size()) || 
        norm_idx.y >= static_cast<int>(vars.size()) || 
        norm_idx.z >= static_cast<int>(vars.size())) {
        return false;
    }

    norm.x = vars[norm_idx.x].f;
    norm.y = vars[norm_idx.y].f;
    norm.z = vars[norm_idx.z].f;

    return true;
}

bool GeoAttrHelper::SetNormal(const GeoAttribute& attr, std::vector<VarValue>& vars,
                              const sm::ivec3& norm_idx, const sm::vec3& norm)
{
    if (norm_idx.x < 0 || 
        norm_idx.y < 0 || 
        norm_idx.z < 0 ||
        norm_idx.x >= static_cast<int>(vars.size()) || 
        norm_idx.y >= static_cast<int>(vars.size()) || 
        norm_idx.z >= static_cast<int>(vars.size())) {
        return false;
    }

    vars[norm_idx.x].f = norm.x;
    vars[norm_idx.y].f = norm.y;
    vars[norm_idx.z].f = norm.z;

    return true;
}

}