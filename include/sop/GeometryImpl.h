#pragma once

#include "sop/GeoAdaptor.h"
#include "sop/GroupMgr.h"
#include "sop/GeoAttribute.h"

#include <functional>

namespace sop
{

class GeometryImpl
{
public:
    GeometryImpl(GeoAdaptor::Type type);

    auto GetAdaptorType() const { return m_adaptor.GetType(); }

    auto& GetGroup() { return m_groups; }
    auto& GetGroup() const { return m_groups; }

    void UpdateByBrush(const model::BrushModel& brush_model);
    void StoreBrush(std::unique_ptr<model::BrushModel>& brush_model);

    void UpdateByAttr();

    auto GetNode() const { return m_adaptor.GetNode(); }

    auto& GetAttr() { return m_attr; }
    auto& GetAttr() const { return m_attr; }

    std::vector<std::shared_ptr<GeoShape>> ToGeoShapes() const;
    void FromGeoShapes(const std::vector<std::shared_ptr<GeoShape>>& shapes);

    // fixme: move to private
public:
    model::BrushModel* GetBrushModel() const {
        return m_adaptor.GetBrushModel();
    }
    std::vector<std::shared_ptr<gs::Shape3D>> GetGeoShapes() const {
        return m_adaptor.GetGeoShapes();
    }

private:
    GeoAdaptor   m_adaptor;

    GroupMgr     m_groups;

    GeoAttribute m_attr;

}; // GeometryImpl

}