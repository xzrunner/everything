#include "sop/node/AttributePromote.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/ParmList.h"

namespace sop
{
namespace node
{

void AttributePromote::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    if (m_attr_name.empty() || m_from_type == m_to_type) {
        return;
    }

    auto& attr = m_geo_impl->GetAttr();
    auto src_list = attr.QueryParmList(m_from_type, m_attr_name);
    if (!src_list) {
        return;
    }

    if (m_from_type == GeoAttrClass::Vertex &&
        m_to_type   == GeoAttrClass::Point)
    {
        auto& src_vts = attr.GetVertices();
        auto& dst_pts = attr.GetPoints();

        auto dst_list = src_list->Clone();
        dst_list->Clear();
        dst_list->Resize(dst_pts.size());
        for (size_t i = 0, n = src_vts.size(); i < n; ++i) {
            dst_list->CopyFrom(src_vts[i]->point->attr_idx, *src_list, i);
        }

        attr.RemoveParmList(m_from_type, m_attr_name);
        assert(dst_list->GetName() == m_attr_name);
        attr.AddParmList(m_to_type, dst_list);
    }
    else if (m_from_type == GeoAttrClass::Point &&
             m_to_type   == GeoAttrClass::Vertex)
    {
        auto& src_pts = attr.GetPoints();
        auto& dst_vts = attr.GetVertices();

        auto dst_list = src_list->Clone();
        dst_list->Clear();
        dst_list->Resize(dst_vts.size());
        for (size_t i = 0, n = dst_vts.size(); i < n; ++i) {
            dst_list->CopyFrom(i, *src_list, dst_vts[i]->point->attr_idx);
        }

        attr.RemoveParmList(m_from_type, m_attr_name);
        assert(dst_list->GetName() == m_attr_name);
        attr.AddParmList(m_to_type, dst_list);
    }

    attr.RemoveParmList(m_from_type, m_attr_name);
}

}
}