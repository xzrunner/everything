#include "everything/GeometryImpl.h"

namespace evt
{

GeometryImpl::GeometryImpl(GeoShapeType type)
    : m_adaptor(type)
{
}

GeometryImpl::GeometryImpl(const GeoShape& shape)
    : m_adaptor(shape)
    , m_attr(shape)
{
}

void GeometryImpl::UpdateByBrush(const model::BrushModel& brush_model)
{
    m_adaptor.UpdateByBrush(const_cast<GeoAttribute&>(m_attr), brush_model);
}

void GeometryImpl::StoreBrush(std::unique_ptr<model::BrushModel>& brush_model)
{
    m_adaptor.StoreBrush(brush_model);
}

void GeometryImpl::UpdateByAttr()
{
    m_adaptor.UpdateByAttr(m_attr);
}

std::unique_ptr<GeoShape> GeometryImpl::ToGeoShape() const
{
    return m_adaptor.ToGeoShape();
}

void GeometryImpl::FromGeoShape(const GeoShape& shape)
{
    m_adaptor.FromGeoShape(shape);
    m_attr.FromGeoShape(shape);
}

model::BrushModel* GeometryImpl::GetBrushModel() const
{
    return m_adaptor.GetBrushModel();
}

std::shared_ptr<gs::Shape3D> GeometryImpl::GetShape() const
{
    return m_adaptor.GetShape();
}

}