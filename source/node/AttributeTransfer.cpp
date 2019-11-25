#include "sop/node/AttributeTransfer.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

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
                const int from_idx = from_attr.QueryAttrIdx(GeoAttrClass::Point, name);
                assert(from_idx >= 0);

                std::vector<VarValue> vars;
                vars.reserve(from_sz);
                for (auto& p : from_attr.GetPoints()) {
                    vars.push_back(VarValue(p->vars[from_idx]));
                }
                to_attr.AddAttr(GeoAttrClass::Point, from_attr.GetAttrDesc(GeoAttrClass::Point)[from_idx], vars);
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
                const int from_idx = from_attr.QueryAttrIdx(GeoAttrClass::Vertex, name);
                assert(from_idx >= 0);

                std::vector<VarValue> vars;
                vars.reserve(from_sz);
                for (auto& v : from_attr.GetVertices()) {
                    vars.push_back(VarValue(v->vars[from_idx]));
                }
                to_attr.AddAttr(GeoAttrClass::Vertex, from_attr.GetAttrDesc(GeoAttrClass::Vertex)[from_idx], vars);
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
                const int from_idx = from_attr.QueryAttrIdx(GeoAttrClass::Primitive, name);
                assert(from_idx >= 0);

                std::vector<VarValue> vars;
                vars.reserve(from_sz);
                for (auto& prim : from_attr.GetPrimtives()) {
                    vars.push_back(VarValue(prim->vars[from_idx]));
                }
                to_attr.AddAttr(GeoAttrClass::Primitive, from_attr.GetAttrDesc(GeoAttrClass::Primitive)[from_idx], vars);
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
                const int from_idx = from_attr.QueryAttrIdx(GeoAttrClass::Detail, name);
                assert(from_idx >= 0);

                std::vector<VarValue> vars;
                vars.reserve(from_sz);
                vars.push_back(VarValue(from_attr.GetDetail().vars[from_idx]));
                to_attr.AddAttr(GeoAttrClass::Detail, from_attr.GetAttrDesc(GeoAttrClass::Detail)[from_idx], vars);
            }
        }
    }
}

}
}