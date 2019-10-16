#include "sop/GeometryImpl.h"

namespace sop
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

const std::vector<he::PolylinePtr>&
GeometryImpl::GetTopoLines() const
{
    return m_adaptor.GetTopoLines();
}

void GeometryImpl::SetTopoLines(const std::vector<he::PolylinePtr>& lines)
{
    m_adaptor.SetTopoLines(lines);
    m_attr.SetTopoLines(lines);
}

}