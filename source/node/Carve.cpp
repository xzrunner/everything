#include "sop/node/Carve.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/ParmList.h"

#include <SM_Calc.h>
#include <halfedge/Polyline.h>

namespace sop
{
namespace node
{

void Carve::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    auto& lines = prev_geo->GetTopoLines();
    if (lines.empty()) {
        return;
    }

    // todo: only support one shape
    auto& prev_line = lines.front();
    // todo: only support polyline now
    if (prev_line->GetPolylines().Size() == 0) {
        return;
    }

    std::vector<sm::vec3> vertices;
    auto polyline = prev_line->GetPolylines().Head();
    assert(polyline);
    auto first_edge = polyline->edge;
    auto curr_edge = first_edge;
    do {
        vertices.push_back(curr_edge->vert->position);
        curr_edge = curr_edge->next;
    } while (curr_edge && curr_edge != first_edge);
    if (vertices.size() < 2) {
        return;
    }

    std::vector<float> each_len;
    each_len.reserve(vertices.size());
    float tot_len = 0;
    for (int i = 0, n = vertices.size() - 1; i < n; ++i) {
        tot_len += sm::dis_pos3_to_pos3(vertices[i], vertices[i+1]);
        each_len.push_back(tot_len);
    }

    // only support first_u and second_u
    float len_min, len_max;
    if (m_first_u < m_second_u) {
        len_min = tot_len * m_first_u;
        len_max = tot_len * m_second_u;
    } else {
        len_min = tot_len * m_second_u;
        len_max = tot_len * m_first_u;
    }

    auto calc_pos = [&](float len, sm::vec3& pos, size_t& idx_int, float& idx_frac)
    {
        for (int i = 0, n = vertices.size() - 1; i < n; ++i)
        {
            auto& curr_len = each_len[i];
            if (len >= curr_len) {
                continue;
            }

            float prev_len = 0.0f;
            if (i > 0) {
                prev_len = each_len[i - 1];
            }
            assert(len >= prev_len && len < curr_len);
            const float p = (len - prev_len) / (curr_len - prev_len);
            pos = vertices[i] + (vertices[i + 1] - vertices[i]) * p;
            idx_int  = i;
            idx_frac = p;
            return;
        }
        assert(fabs(len - each_len.back()) < std::numeric_limits<float>::epsilon());
        pos = vertices.back();
        idx_int  = vertices.size() - 1;
        idx_frac = 0;
    };

    sm::vec3 start, end;
    size_t s_idx_int, e_idx_int;
    float s_idx_frac, e_idx_frac;
    calc_pos(len_min, start, s_idx_int, s_idx_frac);
    calc_pos(len_max, end, e_idx_int, e_idx_frac);
    assert(s_idx_int <= e_idx_int);

    // calc normal
    auto src_norm_list = prev_geo->GetAttr().QueryParmList(GeoAttrClass::Point, GEO_ATTR_NORM);
    sm::vec3 s_norm, e_norm;
    if (src_norm_list)
    {
        auto& data = std::static_pointer_cast<ParmFlt3List>(src_norm_list)->GetAllItems();
        auto& attr = prev_geo->GetAttr();
        if (s_idx_frac == 0)
        {
            s_norm = data[s_idx_int];
        }
        else
        {
            auto prev = data[s_idx_int];
            auto next = data[s_idx_int + 1];
            s_norm = prev + (next - prev) * s_idx_frac;
        }
        if (e_idx_frac == 0)
        {
            e_norm = data[e_idx_int];
        }
        else
        {
            auto prev = data[e_idx_int];
            auto next = data[e_idx_int + 1];
            e_norm = prev + (next - prev) * e_idx_frac;
        }
    }

    std::vector<sm::vec3> dst_vertices;
    dst_vertices.reserve(e_idx_int - s_idx_int + 1);
    dst_vertices.push_back(start);
    for (size_t i = s_idx_int + 1; i <= e_idx_int; ++i) {
        dst_vertices.push_back(vertices[i]);
    }
    if (end != vertices[e_idx_int]) {
        dst_vertices.push_back(end);
    }

    assert(!dst_vertices.empty());
    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Shape);

    std::vector<std::pair<he::TopoID, sm::vec3>> line_vertices;
    std::vector<std::pair<he::TopoID, std::vector<size_t>>> line_polylines;
    line_vertices.reserve(vertices.size());
    for (auto& v : dst_vertices) {
        line_vertices.push_back({ he::TopoID(), v });
    }
    std::vector<size_t> indices(dst_vertices.size());
    for (size_t i = 0, n = dst_vertices.size(); i < n; ++i) {
        indices[i] = i;
    }
    line_polylines.push_back({ he::TopoID(), { indices } });
    auto line = std::make_shared<he::Polyline>(line_vertices, line_polylines);
    m_geo_impl->SetTopoLines({ line });

    auto& dst_lists = const_cast<std::vector<std::shared_ptr<ParmList>>&>(
        m_geo_impl->GetAttr().GetAllParmLists()[static_cast<int>(GeoAttrClass::Point)]
    );
    auto& src_lists = prev_geo->GetAttr().GetAllParmLists()[static_cast<int>(GeoAttrClass::Point)];
    dst_lists.reserve(src_lists.size());
    for (auto& s : src_lists) 
    {
        auto d = s->Clone(false);
        d->Resize(m_geo_impl->GetAttr().GetPoints().size());
        dst_lists.push_back(d);
    }

    // update point attrs
    auto& prev_attr = prev_geo->GetAttr();
    auto& attr = m_geo_impl->GetAttr();
    auto& pts = m_geo_impl->GetAttr().GetPoints();

    assert(src_lists.size() == dst_lists.size());

    auto dst_norm_list = m_geo_impl->GetAttr().QueryParmList(GeoAttrClass::Point, GEO_ATTR_NORM);

    size_t idx = 0;
    if (start == vertices[s_idx_int]) 
    {
        for (size_t i = 0, n = src_lists.size(); i < n; ++i) {
            dst_lists[i]->CopyFrom(idx, *src_lists[i], s_idx_int);
        }
        ++idx;
    }  
    else  
    {
        ++idx;
        if (dst_norm_list && s_idx_frac != 0) {
            auto& data = std::static_pointer_cast<ParmFlt3List>(dst_norm_list)->GetAllItems();
            const_cast<sm::vec3&>(data[idx - 1]) = s_norm;
        }
    }
    for (size_t i = s_idx_int + 1; i <= e_idx_int; ++i) 
    {
        for (size_t i = 0, n = src_lists.size(); i < n; ++i) {
            dst_lists[i]->CopyFrom(idx, *src_lists[i], i);
        }
        ++idx;
    }
    if (end != vertices[e_idx_int])
    {
        if (end == vertices[e_idx_int]) 
        {
            for (size_t i = 0, n = src_lists.size(); i < n; ++i) {
                dst_lists[i]->CopyFrom(idx, *src_lists[i], e_idx_int);
            }
            ++idx;
        } 
        else 
        {
            ++idx;
            if (dst_norm_list && e_idx_frac != 0) {
                auto& data = std::static_pointer_cast<ParmFlt3List>(dst_norm_list)->GetAllItems();
                const_cast<sm::vec3&>(data[idx - 1]) = e_norm;
            }
        }
    }
}

}
}