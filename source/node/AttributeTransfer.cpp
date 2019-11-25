#include "sop/node/AttributeTransfer.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/ParmList.h"

namespace sop
{
namespace node
{

void AttributeTransfer::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto to_geo = NodeHelper::GetInputGeo(*this, IDX_TO_GEO);
    if (!to_geo) {
        return;
    }
    auto from_geo = NodeHelper::GetInputGeo(*this, IDX_FROM_GEO);
    if (!from_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*to_geo);

    auto& from_attr = from_geo->GetAttr();
    auto& to_attr = m_geo_impl->GetAttr();
    if (m_point_attribs && !m_point_attrib_list.empty())
    {
        auto from_sz = from_attr.GetSize(GeoAttrClass::Point);
        if (from_sz == to_attr.GetSize(GeoAttrClass::Point))
        {
            for (auto& name : m_point_attrib_list)
            {
                auto src_list = from_attr.QueryParmList(GeoAttrClass::Point, name);
                assert(src_list);
                to_attr.AddParmList(GeoAttrClass::Point, src_list->Clone());
            }
        }
    }
    if (m_vertex_attribs && !m_vertex_attrib_list.empty())
    {
        auto from_sz = from_attr.GetSize(GeoAttrClass::Vertex);
        if (from_sz == to_attr.GetSize(GeoAttrClass::Vertex))
        {
            for (auto& name : m_vertex_attrib_list)
            {
                auto src_list = from_attr.QueryParmList(GeoAttrClass::Vertex, name);
                assert(src_list);
                to_attr.AddParmList(GeoAttrClass::Vertex, src_list->Clone());
            }
        }
    }
    if (m_primitive_attribs && !m_prim_attrib_list.empty())
    {
        auto from_sz = from_attr.GetSize(GeoAttrClass::Primitive);
        if (from_sz == to_attr.GetSize(GeoAttrClass::Primitive))
        {
            for (auto& name : m_prim_attrib_list)
            {
                auto src_list = from_attr.QueryParmList(GeoAttrClass::Primitive, name);
                assert(src_list);
                to_attr.AddParmList(GeoAttrClass::Primitive, src_list->Clone());
            }
        }
    }
    if (m_detail_attribs && !m_detail_attrib_list.empty())
    {
        auto from_sz = from_attr.GetSize(GeoAttrClass::Detail);
        if (from_sz == to_attr.GetSize(GeoAttrClass::Detail))
        {
            for (auto& name : m_detail_attrib_list)
            {
                auto src_list = from_attr.QueryParmList(GeoAttrClass::Detail, name);
                assert(src_list);
                to_attr.AddParmList(GeoAttrClass::Detail, src_list->Clone());
            }
        }
    }
}

}
}