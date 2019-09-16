#include "everything/GeometryImpl.h"

namespace evt
{

GeometryImpl::GeometryImpl(GeoAdaptor::Type type)
    : m_adaptor(type)
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

std::vector<std::shared_ptr<GeoShape>>
GeometryImpl::ToGeoShapes() const
{
    return m_adaptor.ToGeoShapes();
}

void GeometryImpl::FromGeoShapes(const std::vector<std::shared_ptr<GeoShape>>& shapes)
{
    m_adaptor.FromGeoShapes(shapes);
    m_attr.FromGeoShapes(shapes);
}

}