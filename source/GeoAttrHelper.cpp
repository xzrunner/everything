#include "everything/GeoAttrHelper.h"

#include <SM_Plane.h>

namespace evt
{

sm::vec3 GeoAttrHelper::CalcFaceNormal(const GeoAttribute::Primitive& prim)
{
    assert(prim.vertices.size() > 2);
    sm::Plane plane;
    plane.Build(
        prim.vertices[0]->point->pos,
        prim.vertices[1]->point->pos,
        prim.vertices[2]->point->pos
    );
    return plane.normal;
}

}