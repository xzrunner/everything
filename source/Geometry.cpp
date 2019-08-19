#include "everything/Geometry.h"

namespace evt
{

Geometry::Geometry(GeoAdaptor::DataType type)
    : m_adaptor(type)
{}

void Geometry::Combine(const Geometry& geo, size_t prim_off)
{
    m_groups.Combine(geo.m_groups, prim_off);
}

void Geometry::AddGroup(const std::shared_ptr<Group>& group)
{
    m_groups.Add(group);
}

std::shared_ptr<Group> Geometry::QueryGroup(const std::string& name) const
{
    return m_groups.Query(name);
}

void Geometry::UpdateByBrush(const model::BrushModel& brush_model) const
{
    m_adaptor.UpdateByBrush(const_cast<GeoAttribute&>(m_attr), brush_model);
}

void Geometry::StoreBrush(std::unique_ptr<model::BrushModel>& brush_model)
{
    m_adaptor.StoreBrush(brush_model);
}

void Geometry::UpdateByShape(const std::shared_ptr<gs::Shape3D>& shape)
{
    m_adaptor.UpdateByShape(m_attr, shape);
}

void Geometry::UpdateByAttr()
{
    m_adaptor.UpdateByAttr(m_attr);
}

model::BrushModel* Geometry::GetBrushModel() const
{
    return m_adaptor.GetBrushModel();
}

std::shared_ptr<gs::Shape3D> Geometry::GetShape() const
{
    return m_adaptor.GetShape();
}

}