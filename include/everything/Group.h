#pragma once

#include <string>
#include <vector>

namespace evt
{

enum class GroupType
{
    GuessFromGroup,
    Points,
    Vertices,
    Edges,
    Primitives,
};

struct Group
{
    std::string name;

    GroupType type = GroupType::Primitives;

    std::vector<size_t> items;

    void Clear() { items.clear(); }

}; // Group

}