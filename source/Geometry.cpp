#include "everything/Geometry.h"

namespace evt
{

Geometry::Geometry(GeoShapeType type)
    : m_adaptor(type)
{
}

Geometry::Geometry(const GeoShape& shape)
    : m_adaptor(shape)
    , m_attr(shape)
{
}

void Geometry::AddGroup(const std::shared_ptr<Group>& group)
{
    m_groups.Add(group);
}

std::shared_ptr<Group> Geometry::QueryGroup(const std::string& name) const
{
    return m_groups.Query(name);
}

void Geometry::UpdateByBrush(const model::BrushModel& brush_model)
{
    m_adaptor.UpdateByBrush(const_cast<GeoAttribute&>(m_attr), brush_model);
}

void Geometry::StoreBrush(std::unique_ptr<model::BrushModel>& brush_model)
{
    m_adaptor.StoreBrush(brush_model);
}

void Geometry::UpdateByAttr()
{
    m_adaptor.UpdateByAttr(m_attr);
}

std::unique_ptr<GeoShape> Geometry::ToGeoShape() const
{
    return m_adaptor.ToGeoShape();
}

void Geometry::FromGeoShape(const GeoShape& shape)
{
    m_adaptor.FromGeoShape(shape);
    m_attr.FromGeoShape(shape);
}

model::BrushModel* Geometry::GetBrushModel() const
{
    return m_adaptor.GetBrushModel();
}

}