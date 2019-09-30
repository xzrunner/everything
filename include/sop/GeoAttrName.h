#pragma once

namespace sop
{

struct GeoAttrName
{
    static const constexpr char* const vert_x = "P[x]";
    static const constexpr char* const vert_y = "P[y]";
    static const constexpr char* const vert_z = "P[z]";

    static const constexpr char* const norm_x = "N[x]";
    static const constexpr char* const norm_y = "N[y]";
    static const constexpr char* const norm_z = "N[z]";

    static const constexpr char* const col_x = "Cd[x]";
    static const constexpr char* const col_y = "Cd[y]";
    static const constexpr char* const col_z = "Cd[z]";
};

}