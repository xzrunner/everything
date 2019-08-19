#pragma once

#include "everything/GeoAdaptor.h"
#include "everything/GroupMgr.h"
#include "everything/GeoAttribute.h"

#include <functional>

namespace evt
{

class Geometry
{
public:

public:
    Geometry(GeoAdaptor::DataType type);

    void Combine(const Geometry& geo, size_t prim_off);

    void AddGroup(const std::shared_ptr<Group>& group);
    std::shared_ptr<Group> QueryGroup(const std::string& name) const;

    void UpdateByBrush(const model::BrushModel& brush_model) const;
    void StoreBrush(std::unique_ptr<model::BrushModel>& brush_model);

    void UpdateByShape(const std::shared_ptr<gs::Shape3D>& shape);

    void UpdateByAttr();

    auto GetNode() const { return m_adaptor.GetNode(); }

    auto& GetAttr() { return m_attr; }
    auto& GetAttr() const { return m_attr; }

    // fixme: move to private
public:
    model::BrushModel* GetBrushModel() const;
    std::shared_ptr<gs::Shape3D> GetShape() const;

private:
    GeoAdaptor   m_adaptor;

    GroupMgr     m_groups;

    GeoAttribute m_attr;

}; // Geometry

}