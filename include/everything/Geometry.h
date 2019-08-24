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
    Geometry(GeoShapeType type);
    Geometry(const GeoShape& shape);

    auto& GetGroup() { return m_groups; }
    void AddGroup(const std::shared_ptr<Group>& group);
    std::shared_ptr<Group> QueryGroup(const std::string& name) const;

    void UpdateByBrush(const model::BrushModel& brush_model);
    void StoreBrush(std::unique_ptr<model::BrushModel>& brush_model);

    void UpdateByAttr();

    auto GetNode() const { return m_adaptor.GetNode(); }

    auto& GetAttr() { return m_attr; }
    auto& GetAttr() const { return m_attr; }

    std::unique_ptr<GeoShape> ToGeoShape() const;
    void FromGeoShape(const GeoShape& shape);

    // fixme: move to private
public:
    model::BrushModel* GetBrushModel() const;

private:
    GeoAdaptor   m_adaptor;

    GroupMgr     m_groups;

    GeoAttribute m_attr;

}; // Geometry

}