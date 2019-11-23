#pragma once

#include <string>
#include <vector>

namespace sop
{

enum class GroupType
{
    GuessFromGroup,
    Points,
    Vertices,
    Edges,
    Primitives,
};

enum class GroupTypes
{
    Auto,
    Points,
    Vertices,
    Edges,
    Primitives,
};

enum class GroupMerge
{
    Replace,
    Union,
    Intersect,
    Subtract,
};

class Group
{
public:
    auto& GetName() const { return m_name; }
    void  SetName(const std::string& name) { m_name = name; }

    auto GetType() const { return m_type; }
    void SetType(GroupType type) { m_type = type; }

    size_t Size() const { return m_items.size(); }
    auto& GetItems() const { return m_items; }
    void  SetItems(const std::vector<size_t>& items) { m_items = items; }

    bool Add(size_t item);
    void Clear();

    void Offset(size_t off);

    static void Merge(GroupMerge op, const std::vector<size_t>& src, std::vector<size_t>& dst);

private:
    std::string m_name;

    GroupType m_type = GroupType::Primitives;

    std::vector<size_t> m_items;

}; // Group

}