#pragma once

#include <polymesh3/typedef.h>

#include <vector>

namespace evt
{

struct BrushPart
{
    std::vector<pm3::PointIndex> vertices;
    std::vector<pm3::EdgeIndex>  edges;
    std::vector<size_t>          faces;

    void Clear() {
        vertices.clear();
        edges.clear();
        faces.clear();
    }

}; // BrushPart

struct BrushGroup
{
    std::string name;

    std::vector<BrushPart> parts;

    void Clear() {
        for (auto& p : parts) {
            p.Clear();
        }
    }

}; // BrushGroup
}
