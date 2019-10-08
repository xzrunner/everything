#include "sop/node/Carve.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/GeoAttrHelper.h"

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

    auto lines = prev_geo->GetTopoLines();
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
    auto& props = m_props.GetProps();
    assert(props[FIRST_U].Val().type == VarType::Float
        && props[SECOND_U].Val().type == VarType::Float);
    auto& first_u  = props[FIRST_U].Val().f;
    auto& second_u = props[SECOND_U].Val().f;
    if (first_u < second_u) {
        len_min = tot_len * first_u;
        len_max = tot_len * second_u;
    } else {
        len_min = tot_len * second_u;
        len_max = tot_len * first_u;
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
    sm::ivec3 norm_idx;
    bool has_norm = GeoAttrHelper::QueryNormalIndices(prev_geo->GetAttr(), GeoAttrType::Point, norm_idx);
    sm::vec3 s_norm, e_norm;
    if (has_norm)
    {
        auto& attr = prev_geo->GetAttr();
        if (s_idx_frac == 0)
        {
            GeoAttrHelper::GetNormal(attr, GeoAttrType::Point, s_idx_int, norm_idx, s_norm);
        }
        else
        {
            sm::vec3 prev, next;
            bool b_prev = GeoAttrHelper::GetNormal(attr, GeoAttrType::Point, s_idx_int, norm_idx, prev);
            bool b_next = GeoAttrHelper::GetNormal(attr, GeoAttrType::Point, s_idx_int + 1, norm_idx, next);
            assert(b_prev && b_next);
            s_norm = prev + (next - prev) * s_idx_frac;
        }
        if (e_idx_frac == 0)
        {
            GeoAttrHelper::GetNormal(attr, GeoAttrType::Point, e_idx_int, norm_idx, e_norm);
        }
        else
        {
            sm::vec3 prev, next;
            bool b_prev = GeoAttrHelper::GetNormal(attr, GeoAttrType::Point, e_idx_int, norm_idx, prev);
            bool b_next = GeoAttrHelper::GetNormal(attr, GeoAttrType::Point, e_idx_int + 1, norm_idx, next);
            assert(b_prev && b_next);
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

    // update point attrs
    auto& prev_attr = prev_geo->GetAttr();
    auto& attr = m_geo_impl->GetAttr();
    attr.SetAttrDesc(GeoAttrType::Point,
        prev_attr.GetAttrDesc(GeoAttrType::Point));

    auto default_vars = prev_attr.GetDefaultValues(GeoAttrType::Point);
    auto& prev_pts = prev_attr.GetPoints();
    assert(prev_pts.size() == vertices.size());
    auto& pts = attr.GetPoints();

    size_t idx = 0;
    if (start == vertices[s_idx_int]) {
        pts[idx++]->vars = prev_pts[s_idx_int]->vars;
    }  else  {
        pts[idx++]->vars = default_vars;
        if (has_norm && s_idx_frac != 0) {
            GeoAttrHelper::SetNormal(attr, pts[idx - 1]->vars, norm_idx, s_norm);
        }
    }
    for (size_t i = s_idx_int + 1; i <= e_idx_int; ++i) {
        pts[idx++]->vars = prev_pts[i]->vars;
    }
    if (end != vertices[e_idx_int]) {
        if (end == vertices[e_idx_int]) {
            pts[idx++]->vars = prev_pts[e_idx_int]->vars;
        } else {
            pts[idx++]->vars = default_vars;
            if (has_norm && e_idx_frac != 0) {
                GeoAttrHelper::SetNormal(attr, pts[idx - 1]->vars, norm_idx, e_norm);
            }
        }
    }
}

void Carve::SetFirstU(float u)
{
    u = std::min(1.0f, std::max(0.0f, u));
    if (m_props.SetValue(FIRST_U, Variable(u))) {
        SetDirty(true);
    }
}

void Carve::SetSecondU(float u)
{
    u = std::min(1.0f, std::max(0.0f, u));
    if (m_props.SetValue(SECOND_U, Variable(u))) {
        SetDirty(true);
    }
}

void Carve::SetFirstV(float v)
{
    v = std::min(1.0f, std::max(0.0f, v));
    if (m_props.SetValue(FIRST_V, Variable(v))) {
        SetDirty(true);
    }

}

void Carve::SetSecondV(float v)
{
    v = std::min(1.0f, std::max(0.0f, v));
    if (m_props.SetValue(SECOND_V, Variable(v))) {
        SetDirty(true);
    }
}

void Carve::InitProps()
{
    m_props.Assign(FIRST_U,  PropNames[FIRST_U],  Variable(0.0f));
    m_props.Assign(SECOND_U, PropNames[SECOND_U], Variable(1.0f));
    m_props.Assign(FIRST_V,  PropNames[FIRST_V],  Variable(0.0f));
    m_props.Assign(SECOND_V, PropNames[SECOND_V], Variable(1.0f));
}

}
}