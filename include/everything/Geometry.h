#pragma once

#include <SM_Vector.h>
#include <node0/typedef.h>

#include <vector>
#include <string>
#include <map>
#include <functional>

namespace he { struct Edge; }
namespace model { class BrushModel; }
namespace pm3 { class Polytope; struct Face; }
namespace gs { class Shape3D; }

namespace evt
{

class Geometry
{
public:
    enum class GroupType
    {
        Points,
        Vertices,
        Edges,
        Face,
    };

    struct Group
    {
        std::string name;

        GroupType type = GroupType::Face;

        std::vector<size_t> items;

        void Clear() { items.clear(); }

    }; // Group

    enum class DataType
    {
        Brush,
        Shape,
    };

public:
    Geometry(DataType type);
    Geometry(const Geometry& geo);
    Geometry& operator = (const Geometry& geo);

    void TraversePoints(std::function<bool(sm::vec3&, bool& dirty)> func,
        const std::string& group_name = "") const;
    void TraverseEdges(std::function<bool(const sm::vec3& begin, const sm::vec3& end)> func,
        const std::string& group_name = "") const;
    void TraverseFaces(std::function<bool(pm3::Polytope&, size_t face_idx, bool& dirty)> func,
        const std::string& group_name = "") const;

    void Combine(const Geometry& geo, size_t face_offset);

    void AddGroup(const std::shared_ptr<Group>& group);
    std::shared_ptr<Group> QueryGroup(const std::string& name) const;
    //void RenameGroup(const std::string& src, const std::string& dst);

    auto GetNode() const { return m_node; }

    void UpdateModel(const model::BrushModel& brush_model) const;
    void StoreBrush(std::unique_ptr<model::BrushModel>& brush_model);

    // fixme: move to private
public:
    model::BrushModel* GetBrushModel() const;
    std::shared_ptr<gs::Shape3D> GetShape() const;

private:
    void CopyGroups(const Geometry& geo);

private:
    DataType m_type;

    n0::SceneNodePtr m_node = nullptr;

    std::map<std::string, std::shared_ptr<Group>> m_groups;

}; // Geometry

}