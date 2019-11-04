#pragma once

namespace GEO { class Mesh; }

namespace sop
{

class GeoAttribute;

class GeogramHelper
{
public:
    static void BuildMesh(GEO::Mesh& dst, const GeoAttribute& src);

    static void LoadTexcoordsFromMesh(GeoAttribute& dst, const GEO::Mesh& src);
    static void LoadTexcoordsToMesh(GEO::Mesh& dst, const GeoAttribute& src);

}; // GeogramHelper

}