#pragma once

#include "everything/GroupMgr.h"
#include "everything/Geometry.h"

#include <node0/typedef.h>

#include <functional>

namespace model { class BrushModel; }
namespace pm3 { class Polytope; }
namespace gs { class Shape3D; }

namespace evt
{

class GeometryNode
{
public:
    enum class DataType
    {
        Brush,
        Shape,
    };

public:
    GeometryNode(DataType type);
    GeometryNode(const GeometryNode& geo);
    GeometryNode& operator = (const GeometryNode& geo);

    void TraversePoints(std::function<bool(sm::vec3&, bool& dirty)> func,
        const std::string& group_name = "") const;
    void TraverseEdges(std::function<bool(const sm::vec3& begin, const sm::vec3& end)> func,
        const std::string& group_name = "") const;
    void TraverseFaces(std::function<bool(pm3::Polytope&, size_t face_idx, bool& dirty)> func,
        const std::string& group_name = "") const;

    void Combine(const GeometryNode& geo, size_t prim_off);

    void AddGroup(const std::shared_ptr<Group>& group);
    std::shared_ptr<Group> QueryGroup(const std::string& name) const;

    auto GetNode() const { return m_node; }

    void UpdateModel(const model::BrushModel& brush_model) const;
    void StoreBrush(std::unique_ptr<model::BrushModel>& brush_model);

    // fixme: move to private
public:
    model::BrushModel* GetBrushModel() const;
    std::shared_ptr<gs::Shape3D> GetShape() const;

private:
    DataType m_type;

    n0::SceneNodePtr m_node = nullptr;

    GroupMgr m_groups;

    Geometry m_geo;

}; // GeometryNode

}