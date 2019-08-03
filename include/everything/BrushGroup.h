#pragma once

#include <polymesh3/typedef.h>

#include <vector>

namespace evt
{

struct BrushGroup
{
    std::string name;

    std::vector<pm3::BrushVertex> vertices;
    std::vector<pm3::BrushEdge>   edges;
    std::vector<size_t>           faces;

}; // BrushGroup
}
