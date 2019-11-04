#include "sop/GeoAttribute.h"

#include <halfedge/Polyline.h>
#include <SM_Plane.h>

#include <map>
#include <set>

namespace sop
{

GeoAttribute::GeoAttribute(const GeoAttribute& attr)
{
    operator = (attr);
}

GeoAttribute& GeoAttribute::operator = (const GeoAttribute& attr)
{
    // prepare

    std::map<std::shared_ptr<Point>, size_t> point2idx;
    for (size_t i = 0, n = attr.m_points.size(); i < n; ++i) {
        point2idx.insert({ attr.m_points[i], i });
    }

    std::map<std::shared_ptr<Vertex>, size_t> vertex2idx;
    for (size_t i = 0, n = attr.m_vertices.size(); i < n; ++i) {
        vertex2idx.insert({ attr.m_vertices[i], i });
    }

    std::map<std::shared_ptr<Primitive>, size_t> prim2idx;
    for (size_t i = 0, n = attr.m_primtives.size(); i < n; ++i) {
        prim2idx.insert({ attr.m_primtives[i], i });
    }

    // points
    m_points.reserve(attr.m_points.size());
    for (auto& p : attr.m_points) {
        m_points.push_back(std::make_shared<Point>(*p));
    }

    // vertices
    m_vertices.reserve(attr.m_vertices.size());
    for (auto& v : attr.m_vertices) {
        m_vertices.push_back(std::make_shared<Vertex>(*v));
    }

    // primitives
    m_primtives.reserve(attr.m_primtives.size());
    for (auto& prim : attr.m_primtives) {
        m_primtives.push_back(std::make_shared<Primitive>(*prim));
    }

    // detail
    m_detail = attr.m_detail;

    // connect
    for (size_t i = 0, n = attr.m_vertices.size(); i < n; ++i)
    {
        auto& src = attr.m_vertices[i];
        auto& dst = m_vertices[i];

        auto itr_p = point2idx.find(src->point);
        assert(itr_p != point2idx.end());
        auto itr_prim = prim2idx.find(src->prim.lock());
        assert(itr_prim != prim2idx.end());
        dst->point = m_points[itr_p->second];
        dst->prim = m_primtives[itr_prim->second];
    }
    for (size_t i = 0, n = attr.m_primtives.size(); i < n; ++i)
    {
        auto& src = attr.m_primtives[i];
        auto& dst = m_primtives[i];

        dst->vertices.reserve(src->vertices.size());
        for (auto& v : src->vertices)
        {
            auto itr = vertex2idx.find(v);
            assert(itr != vertex2idx.end());
            dst->vertices.push_back(m_vertices[itr->second]);
        }
    }

    for (int i = 0, n = static_cast<int>(GeoAttrClass::MaxTypeNum); i < n; ++i) {
        m_var_descs[i] = attr.m_var_descs[i];
    }

    SetupAABB();

    return *this;
}

size_t GeoAttribute::GetSize(GeoAttrClass cls) const
{
    switch (cls)
    {
    case GeoAttrClass::Point:
        return m_points.size();
    case GeoAttrClass::Vertex:
        return m_vertices.size();
    case GeoAttrClass::Primitive:
        return m_primtives.size();
    case GeoAttrClass::Detail:
        return 1;
    default:
        assert(0);
        return 0;
    }
}

void GeoAttribute::RemoveItems(GeoAttrClass cls, const std::vector<bool>& del_flags, bool del_unused_pt)
{
    bool dirty = false;
    for (auto f : del_flags) {
        if (f) {
            dirty = true;
            break;
        }
    }
    if (!dirty) {
        return;
    }

    switch (cls)
    {
    case GeoAttrClass::Point:
    {
        assert(m_points.size() == del_flags.size());
        int idx = 0;
        for (auto itr = m_points.begin(); itr != m_points.end(); )
        {
            if (del_flags[idx++]) {
                itr = m_points.erase(itr);
            } else {
                ++itr;
            }
        }

        UpdatePointsChanged(del_unused_pt);
    }
        break;
    case GeoAttrClass::Vertex:
    {
        assert(m_vertices.size() == del_flags.size());
        int idx = 0;
        for (auto itr = m_vertices.begin(); itr != m_vertices.end(); )
        {
            if (del_flags[idx++]) {
                itr = m_vertices.erase(itr);
            } else {
                ++itr;
            }
        }

        UpdateVerticesChanged(del_unused_pt);
    }
        break;
    case GeoAttrClass::Primitive:
    {
        assert(m_primtives.size() == del_flags.size());
        int idx = 0;
        for (auto itr = m_primtives.begin(); itr != m_primtives.end(); )
        {
            if (del_flags[idx++]) {
                itr = m_primtives.erase(itr);
            } else {
                ++itr;
            }
        }

        UpdatePrimitivesChanged(del_unused_pt);
    }
        break;
    default:
        assert(0);
    }
}

void GeoAttribute::ChangePointsOrder(const std::vector<size_t>& order)
{
    assert(order.size() == m_points.size());
    if (order.size() != m_points.size()) {
        return;
    }

    std::vector<std::shared_ptr<Point>> points(m_points.size(), nullptr);
    for (int i = 0, n = order.size(); i < n; ++i) {
        points[i] = m_points[order[i]];
    }

    bool valid = true;
    for (auto& p : points) {
        if (!p) {
            valid = false;
            break;
        }
    }
    assert(valid);
    if (!valid) {
        return;
    }

    m_points = points;

    SetupAttrIndices();
}

void GeoAttribute::AddAttr(GeoAttrClass cls, const VarDesc& var_desc,
                           const std::vector<VarValue>& var_list)
{
    assert(cls >= static_cast<GeoAttrClass>(0)
        && cls < GeoAttrClass::MaxTypeNum);

    int idx = -1;
    auto& descs = m_var_descs[static_cast<int>(cls)];
    for (int i = 0, n = descs.size(); i < n; ++i) {
        if (descs[i].GetName() == var_desc.GetName()) {
            idx = i;
            descs[i].SetType(var_desc.GetType());
            break;
        }
    }

    if (idx >= 0)
    {
        switch (cls)
        {
        case GeoAttrClass::Point:
            assert(var_list.size() == m_points.size());
            for (int i = 0, n = var_list.size(); i < n; ++i) {
                m_points[i]->vars[idx] = var_list[i];
            }
            break;
        case GeoAttrClass::Vertex:
            assert(var_list.size() == m_vertices.size());
            for (int i = 0, n = var_list.size(); i < n; ++i) {
                m_vertices[i]->vars[idx] = var_list[i];
            }
            break;
        case GeoAttrClass::Primitive:
            assert(var_list.size() == m_primtives.size());
            for (int i = 0, n = var_list.size(); i < n; ++i) {
                m_primtives[i]->vars[idx] = var_list[i];
            }
            break;
        case GeoAttrClass::Detail:
            assert(var_list.size() == 1);
            m_detail.vars[idx] = var_list[0];
            break;
        default:
            assert(0);
        }
    }
    else
    {
        descs.push_back(var_desc);
        switch (cls)
        {
        case GeoAttrClass::Point:
            assert(var_list.size() == m_points.size());
            for (int i = 0, n = var_list.size(); i < n; ++i) {
                m_points[i]->vars.push_back(var_list[i]);
            }
            break;
        case GeoAttrClass::Vertex:
            assert(var_list.size() == m_vertices.size());
            for (int i = 0, n = var_list.size(); i < n; ++i) {
                m_vertices[i]->vars.push_back(var_list[i]);
            }
            break;
        case GeoAttrClass::Primitive:
            assert(var_list.size() == m_primtives.size());
            for (int i = 0, n = var_list.size(); i < n; ++i) {
                m_primtives[i]->vars.push_back(var_list[i]);
            }
            break;
        case GeoAttrClass::Detail:
            assert(var_list.size() == 1);
            m_detail.vars.push_back(var_list[0]);
            break;
        default:
            assert(0);
        }
    }
}

Variable GeoAttribute::QueryAttr(GeoAttrClass cls, const std::string& name, size_t index) const
{
    if (cls == GeoAttrClass::Point && name == GeoAttrNames[GEO_ATTR_POS])
    {
        if (index >= 0 && index < m_points.size()) {
            return Variable(m_points[index]->pos);
        } else {
            return Variable();
        }
    }

    assert(cls >= static_cast<GeoAttrClass>(0)
        && cls < GeoAttrClass::MaxTypeNum);

    int attr_idx = -1;
    auto& descs = m_var_descs[static_cast<int>(cls)];
    for (int i = 0, n = descs.size(); i < n; ++i) {
        if (descs[i].GetName() == name) {
            attr_idx = i;
            break;
        }
    }
    if (attr_idx < 0) {
        return Variable();
    }

    VarValue val;
    switch (cls)
    {
    case GeoAttrClass::Point:
        if (index >= m_points.size()) {
            return Variable();
        }
        assert(m_points[index]->vars.size() == m_var_descs[static_cast<int>(GeoAttrClass::Point)].size());
        val = m_points[index]->vars[attr_idx];
        break;
    case GeoAttrClass::Vertex:
        if (index >= m_vertices.size()) {
            return Variable();
        }
        assert(m_vertices[index]->vars.size() == m_var_descs[static_cast<int>(GeoAttrClass::Vertex)].size());
        val = m_vertices[index]->vars[attr_idx];
        break;
    case GeoAttrClass::Primitive:
        if (index >= m_primtives.size()) {
            return Variable();
        }
        assert(m_primtives[index]->vars.size() == m_var_descs[static_cast<int>(GeoAttrClass::Primitive)].size());
        val = m_primtives[index]->vars[attr_idx];
        break;
    case GeoAttrClass::Detail:
        val = m_detail.vars[attr_idx];
        break;
    default:
        return Variable();
    }

    switch (descs[attr_idx].GetType())
    {
    case GeoAttrType::Bool:
        return Variable(val.b);
    case GeoAttrType::Int:
        return Variable(val.i);
    case GeoAttrType::Float:
        return Variable(val.f);
    case GeoAttrType::Double:
        return Variable(val.d);
    case GeoAttrType::Vector:
        return Variable(*static_cast<const sm::vec3*>(val.p));
    default:
        assert(0);
        return Variable();
    }
}

void GeoAttribute::Combine(const GeoAttribute& attr)
{
    CombineTopoID(attr);
    CombineBrushID(attr);

    CombinePoints(attr);
    CombineVertices(attr);
    CombinePrimitives(attr);
    CombineDetail(attr);

    SetupAttrIndices();
    SetupAABB();
}

void GeoAttribute::SetTopoLines(const std::vector<he::PolylinePtr>& lines)
{
    Clear();

    std::map<he::Vertex*, std::shared_ptr<GeoAttribute::Point>> map_pt;

    // Point
    for (auto line : lines)
    {
        auto& vts = line->GetVertices();
        auto first_point = vts.Head();
        auto curr_point = first_point;
        do {
            auto dst = std::make_shared<GeoAttribute::Point>(curr_point->position);
            dst->topo_id = curr_point->ids;
            m_points.push_back(dst);
            map_pt.insert({ curr_point, dst });

            curr_point = curr_point->linked_next;
        } while (curr_point != first_point);
    }

    // Polyline
    for (auto line : lines)
    {
        auto& src_lines = line->GetPolylines();
        auto first_line = src_lines.Head();
        if (!first_line) {
            continue;
        }
        auto curr_line = first_line;
        do {
            auto first_edge = curr_line->edge;
            auto curr_edge = first_edge;

            auto prim = std::make_shared<GeoAttribute::Primitive>(GeoAttribute::Primitive::Type::PolygonCurves);
            do {
                auto dst_v = std::make_shared<GeoAttribute::Vertex>();
                auto itr = map_pt.find(curr_edge->vert);
                assert(itr != map_pt.end());
                dst_v->point = itr->second;
                dst_v->prim  = prim;
                m_vertices.push_back(dst_v);
                prim->vertices.push_back(dst_v);

                curr_edge = curr_edge->next;
            } while (curr_edge && curr_edge != first_edge);

            prim->topo_id = curr_line->ids;
            m_primtives.push_back(prim);

            curr_line = curr_line->linked_next;
        } while (curr_line && curr_line != first_line);
    }

    SetupAttrIndices();
    SetupAABB();
}

std::vector<VarValue> GeoAttribute::GetDefaultValues(GeoAttrClass cls) const
{
    std::vector<VarValue> ret;

    assert(cls >= static_cast<GeoAttrClass>(0)
        && cls < GeoAttrClass::MaxTypeNum);

    auto& descs = m_var_descs[static_cast<int>(cls)];
    ret.reserve(descs.size());
    for (auto& d : descs)
    {
        switch (d.GetType())
        {
        case GeoAttrType::Bool:
            ret.push_back(VarValue(false));
            break;
        case GeoAttrType::Int:
            ret.push_back(VarValue(0));
            break;
        case GeoAttrType::Float:
            ret.push_back(VarValue(0.0f));
            break;
        case GeoAttrType::Double:
            ret.push_back(VarValue(0.0));
            break;
        case GeoAttrType::Vector:
        case GeoAttrType::Float3:
            ret.push_back(VarValue(sm::vec3()));
            break;
        default:
            assert(0);
        }
    }

    return ret;
}

int GeoAttribute::QueryAttrIdx(GeoAttrClass cls, GeoAttr attr) const
{
    return QueryAttrIdx(cls, GeoAttrNames[attr]);
}

int GeoAttribute::QueryAttrIdx(GeoAttrClass cls, const std::string& name) const
{
    auto& desc = GetAttrDesc(cls);
    for (int i = 0, n = desc.size(); i < n; ++i) {
        if (desc[i].GetName() == name) {
            return i;
        }
    }
    return -1;
}

void GeoAttribute::Clear()
{
    m_points.clear();
    m_vertices.clear();
    m_primtives.clear();
    m_detail.vars.clear();

    for (auto& desc : m_var_descs) {
        desc.clear();
    }

    m_aabb.MakeEmpty();
}

void GeoAttribute::SetupAttrIndices()
{
    for (size_t i = 0, n = m_points.size(); i < n; ++i) {
        m_points[i]->attr_idx = i;
    }
    for (size_t i = 0, n = m_vertices.size(); i < n; ++i) {
        m_vertices[i]->attr_idx = i;
    }
    for (size_t i = 0, n = m_primtives.size(); i < n; ++i) {
        m_primtives[i]->attr_idx = i;
    }
}

void GeoAttribute::SetupAABB()
{
    m_aabb.MakeEmpty();
    for (auto& p : m_points) {
        m_aabb.Combine(p->pos);
    }
}

void GeoAttribute::CombineAttrDesc(const GeoAttribute& attr, GeoAttrClass cls,
                                   std::vector<uint32_t>& indices, std::vector<VarValue>& default_vars)
{
    auto& d_desc = m_var_descs[static_cast<int>(cls)];
    const auto& s_desc = attr.m_var_descs[static_cast<int>(cls)];

    indices.clear();
    for (int i = 0, n = d_desc.size(); i < n; ++i) {
        indices.push_back((static_cast<uint32_t>(i) << 16) | 0x0000ffff);
    }
    for (int i = 0, n = s_desc.size(); i < n; ++i)
    {
        bool find = false;
        for (int j = 0, m = d_desc.size(); j < m; ++j)
        {
            if (s_desc[i].GetName() == d_desc[j].GetName()) {
                indices[j] = (indices[j] & 0xffff0000) | static_cast<uint32_t>(i);
                find = true;
            }
        }
        if (!find) {
            d_desc.push_back(s_desc[i]);
            indices.push_back(0xffff0000 | static_cast<uint32_t>(i));
        }
    }

    default_vars = GetDefaultValues(cls);
}

void GeoAttribute::CombineTopoID(const GeoAttribute& attr)
{
    // prepare unique set
    std::set<size_t> src_pt, src_prim;
    std::set<size_t> dst_pt, dst_prim;
    for (auto& p : attr.GetPoints()) {
        for (auto& id : p->topo_id.Path()) {
            src_pt.insert(id);
        }
    }
    for (auto& prim : attr.GetPrimtives()) {
        for (auto& id : prim->topo_id.Path()) {
            src_prim.insert(id);
        }
    }
    for (auto& p : GetPoints()) {
        for (auto& id : p->topo_id.Path()) {
            dst_pt.insert(id);
        }
    }
    for (auto& prim : GetPrimtives()) {
        for (auto& id : prim->topo_id.Path()) {
            dst_prim.insert(id);
        }
    }

    // next id
    size_t next_pt_id = 0, next_prim_id = 0;
    if (!src_pt.empty()) {
        next_pt_id = std::max(next_pt_id, *src_pt.rbegin() + 1);
    }
    if (!dst_pt.empty()) {
        next_pt_id = std::max(next_pt_id, *dst_pt.rbegin() + 1);
    }
    if (!src_prim.empty()) {
        next_prim_id = std::max(next_prim_id, *src_prim.rbegin() + 1);
    }
    if (!dst_prim.empty()) {
        next_prim_id = std::max(next_prim_id, *dst_prim.rbegin() + 1);
    }

    // update points id
    {
        auto s_itr = src_pt.begin();
        auto d_itr = dst_pt.begin();
        while (s_itr != src_pt.end() && d_itr != dst_pt.end())
        {
            if (*s_itr < *d_itr)
            {
                ++s_itr;
            }
            else if (*s_itr > *d_itr)
            {
                ++d_itr;
            }
            else
            {
                auto new_id = next_pt_id++;
                for (auto& p : GetPoints()) {
                    p->topo_id.Replace(*s_itr, new_id);
                }
                ++s_itr;
                ++d_itr;
            }
        }
    }

    // update prims id
    {
        auto s_itr = src_prim.begin();
        auto d_itr = dst_prim.begin();
        while (s_itr != src_prim.end() && d_itr != dst_prim.end())
        {
            if (*s_itr < *d_itr)
            {
                ++s_itr;
            }
            else if (*s_itr > *d_itr)
            {
                ++d_itr;
            }
            else
            {
                auto new_id = next_prim_id++;
                for (auto& p : GetPrimtives()) {
                    p->topo_id.Replace(*s_itr, new_id);
                }
                ++s_itr;
                ++d_itr;
            }
        }
    }
}

void GeoAttribute::CombineBrushID(const GeoAttribute& attr)
{
    // prepare unique set
    std::set<size_t> src, dst;
    for (auto& p : attr.GetPoints()) {
        src.insert(p->prim_id);
    }
    for (auto& p : GetPoints()) {
        dst.insert(p->prim_id);
    }

    // next id
    size_t next_id = 0;
    if (!src.empty()) {
        next_id = std::max(next_id, *src.rbegin() + 1);
    }
    if (!dst.empty()) {
        next_id = std::max(next_id, *dst.rbegin() + 1);
    }

    // update id
    {
        auto s_itr = src.begin();
        auto d_itr = dst.begin();
        while (s_itr != src.end() && d_itr != dst.end())
        {
            if (*s_itr < *d_itr)
            {
                ++s_itr;
            }
            else if (*s_itr > *d_itr)
            {
                ++d_itr;
            }
            else
            {
                auto new_id = next_id++;
                for (auto& p : GetPoints()) {
                    if (p->prim_id == *s_itr) {
                        p->prim_id = new_id;
                    }
                }
                for (auto& prim : GetPrimtives()) {
                    if (prim->prim_id == *s_itr) {
                        prim->prim_id = new_id;
                    }
                }
                ++s_itr;
                ++d_itr;
            }
        }
    }
}

void GeoAttribute::CombinePoints(const GeoAttribute& attr)
{
    std::vector<uint32_t> indices;
    std::vector<VarValue> default_vars;
    CombineAttrDesc(attr, GeoAttrClass::Point, indices, default_vars);

    for (auto& p : m_points)
    {
        auto vars = default_vars;
        for (size_t i = 0, n = indices.size(); i < n; ++i)
        {
            uint32_t idx = indices[i] >> 16;
            if (idx != 0xffff) {
                vars[i] = p->vars[idx];
            }
        }
        p->vars = vars;
    }
    for (auto& p : attr.m_points)
    {
        auto vars = default_vars;
        for (size_t i = 0, n = indices.size(); i < n; ++i)
        {
            uint32_t idx = indices[i] & 0x0000ffff;
            if (idx != 0xffff) {
                vars[i] = p->vars[idx];
            }
        }
        auto point = std::make_shared<Point>(p->pos);
        point->vars = vars;
        point->topo_id = p->topo_id;
        point->prim_id = p->prim_id;
        m_points.push_back(point);
    }
}

void GeoAttribute::CombineVertices(const GeoAttribute& attr)
{
    std::vector<uint32_t> indices;
    std::vector<VarValue> default_vars;
    CombineAttrDesc(attr, GeoAttrClass::Vertex, indices, default_vars);

    for (auto& v : m_vertices)
    {
        auto vars = default_vars;
        for (size_t i = 0, n = indices.size(); i < n; ++i)
        {
            uint32_t idx = indices[i] >> 16;
            if (idx != 0xffff) {
                vars[i] = v->vars[idx];
            }
        }
        v->vars = vars;
    }

    std::map<std::shared_ptr<Point>, size_t> point2idx;
    for (size_t i = 0, n = attr.m_points.size(); i < n; ++i) {
        point2idx.insert({ attr.m_points[i], i });
    }

    size_t ori_pt_count = m_points.size() - attr.m_points.size();
    for (auto& v : attr.m_vertices)
    {
        auto vertex = std::make_shared<Vertex>();

        auto vars = default_vars;
        for (size_t i = 0, n = indices.size(); i < n; ++i)
        {
            uint32_t idx = indices[i] & 0x0000ffff;
            if (idx != 0xffff) {
                vars[i] = v->vars[idx];
            }
        }
        vertex->vars = vars;

        auto itr = point2idx.find(v->point);
        assert(itr != point2idx.end());
        vertex->point = m_points[ori_pt_count + itr->second];

        m_vertices.push_back(vertex);
    }
}

void GeoAttribute::CombinePrimitives(const GeoAttribute& attr)
{
    std::vector<uint32_t> indices;
    std::vector<VarValue> default_vars;
    CombineAttrDesc(attr, GeoAttrClass::Primitive, indices, default_vars);

    for (auto& prim : m_primtives)
    {
        auto vars = default_vars;
        for (size_t i = 0, n = indices.size(); i < n; ++i)
        {
            uint32_t idx = indices[i] >> 16;
            if (idx != 0xffff) {
                vars[i] = prim->vars[idx];
            }
        }
        prim->vars = vars;
    }

    std::map<std::shared_ptr<Vertex>, size_t> vert2idx;
    for (size_t i = 0, n = attr.m_vertices.size(); i < n; ++i) {
        vert2idx.insert({ attr.m_vertices[i], i });
    }

    size_t ori_vt_count = m_vertices.size() - attr.m_vertices.size();
    for (auto& src_prim : attr.m_primtives)
    {
        auto dst_prim = std::make_shared<Primitive>(src_prim->type);

        auto vars = default_vars;
        for (size_t i = 0, n = indices.size(); i < n; ++i)
        {
            uint32_t idx = indices[i] & 0x0000ffff;
            if (idx != 0xffff) {
                vars[i] = src_prim->vars[idx];
            }
        }
        dst_prim->vars = vars;

        dst_prim->vertices.reserve(src_prim->vertices.size());
        for (auto& src_v : src_prim->vertices)
        {
            assert(src_v->point->prim_id == src_prim->prim_id);
            auto itr = vert2idx.find(src_v);
            assert(itr != vert2idx.end());
            auto v = m_vertices[ori_vt_count + itr->second];
            dst_prim->vertices.push_back(v);
            v->prim = dst_prim;
        }

        dst_prim->topo_id = src_prim->topo_id;
        dst_prim->prim_id = src_prim->prim_id;

        m_primtives.push_back(dst_prim);
    }
}

void GeoAttribute::CombineDetail(const GeoAttribute& attr)
{
    std::vector<uint32_t> indices;
    std::vector<VarValue> default_vars;
    CombineAttrDesc(attr, GeoAttrClass::Detail, indices, default_vars);

    std::vector<VarValue> vars;
    vars.reserve(indices.size());
    for (auto& idx : indices)
    {
        uint32_t old_idx = idx >> 16;
        uint32_t new_idx = idx & 0x0000ffff;
        if (new_idx != 0xffff) {
            vars.push_back(attr.m_detail.vars[new_idx]);
        } else {
            assert(old_idx != 0xffff);
            vars.push_back(attr.m_detail.vars[old_idx]);
        }
    }
    assert(vars.size() == default_vars.size());

    m_detail.vars = vars;
}

void GeoAttribute::UpdatePointsChanged(bool del_unused_pt)
{
    std::set<std::shared_ptr<Point>> p_set;
    for (auto& p : m_points) {
        p_set.insert(p);
    }

    bool v_dirty = false;
    std::set<std::shared_ptr<Vertex>> v_set;
    for (auto itr = m_vertices.begin(); itr != m_vertices.end(); )
    {
        if (p_set.find((*itr)->point) == p_set.end()) {
            v_dirty = true;
            itr = m_vertices.erase(itr);
        } else {
            v_set.insert(*itr);
            ++itr;
        }
    }

    if (v_dirty) {
        UpdateVerticesChanged(del_unused_pt);
    }

    SetupAttrIndices();
    SetupAABB();
}

void GeoAttribute::UpdateVerticesChanged(bool del_unused_pt)
{
    if (del_unused_pt)
    {
        std::set<std::shared_ptr<Point>>  p_set;
        for (auto v : m_vertices) {
            p_set.insert(v->point);
        }

        bool p_dirty = false;
        for (auto itr = m_points.begin(); itr != m_points.end(); )
        {
            if (p_set.find(*itr) == p_set.end()) {
                p_dirty = true;
                itr = m_points.erase(itr);
            }
            else {
                ++itr;
            }
        }
        if (p_dirty) {
            UpdatePointsChanged(del_unused_pt);
        }
    }

    std::set<std::shared_ptr<Vertex>> v_set;
    for (auto v : m_vertices) {
        v_set.insert(v);
    }

    bool prim_dirty = false;
    for (auto itr = m_primtives.begin(); itr != m_primtives.end(); )
    {
        bool valid = true;
        for (auto v : (*itr)->vertices) {
            if (v_set.find(v) == v_set.end()) {
                valid = false;
                break;
            }
        }
        if (!valid) {
            prim_dirty = true;
            itr = m_primtives.erase(itr);
        } else {
            ++itr;
        }
    }
    if (prim_dirty) {
        UpdatePrimitivesChanged(del_unused_pt);
    }
}

void GeoAttribute::UpdatePrimitivesChanged(bool del_unused_pt)
{
    std::set<std::shared_ptr<Vertex>> v_set;
    for (auto prim : m_primtives) {
        for (auto v : prim->vertices) {
            v_set.insert(v);
        }
    }

    bool v_dirty = false;
    for (auto itr = m_vertices.begin(); itr != m_vertices.end(); )
    {
        if (v_set.find(*itr) == v_set.end()) {
            itr = m_vertices.erase(itr);
            v_dirty = true;
        } else {
            ++itr;
        }
    }
    if (v_dirty) {
        UpdateVerticesChanged(del_unused_pt);
    }
}

//////////////////////////////////////////////////////////////////////////
// struct GeoAttribute::Point
//////////////////////////////////////////////////////////////////////////

GeoAttribute::Point::Point(const sm::vec3& pos)
    : pos(pos)
{
}

//////////////////////////////////////////////////////////////////////////
// struct GeoAttribute::Vertex
//////////////////////////////////////////////////////////////////////////

GeoAttribute::Vertex::Vertex(const Vertex& vert)
{
    operator = (vert);
}

GeoAttribute::Vertex&
GeoAttribute::Vertex::operator = (const Vertex& vert)
{
    vars     = vert.vars;
    attr_idx = vert.attr_idx;

    return *this;
}

//////////////////////////////////////////////////////////////////////////
// struct GeoAttribute::Primitive
//////////////////////////////////////////////////////////////////////////

GeoAttribute::Primitive::Primitive(Type type)
    : type(type)
{
}

GeoAttribute::Primitive::Primitive(const Primitive& prim)
{
    operator = (prim);
}

GeoAttribute::Primitive&
GeoAttribute::Primitive::operator = (const Primitive& prim)
{
    type     = prim.type;
    vars     = prim.vars;
    attr_idx = prim.attr_idx;
    topo_id  = prim.topo_id;
    prim_id  = prim.prim_id;
    return *this;
}

sm::vec3 GeoAttribute::Primitive::CalcNormal() const
{
    assert(vertices.size() > 2);
    sm::Plane plane;
    plane.Build(
        vertices[0]->point->pos,
        vertices[1]->point->pos,
        vertices[2]->point->pos
    );
    return plane.normal;
}

//////////////////////////////////////////////////////////////////////////
// struct GeoAttribute::VarDesc
//////////////////////////////////////////////////////////////////////////

GeoAttribute::VarDesc::VarDesc(GeoAttr attr)
    : m_attr(attr)
    , m_name(GeoAttrNames[attr])
    , m_type(GeoAttrTypes[attr])
{
}

GeoAttribute::VarDesc::VarDesc(const std::string& name, GeoAttrType type)
    : m_attr(GEO_ATTR_UNKNOWN)
    , m_name(name)
    , m_type(type)
{
    for (int i = 0; i < GEO_ATTR_UNKNOWN; ++i) {
        if (GeoAttrNames[i] == name) {
            m_attr = static_cast<GeoAttr>(i);
            break;
        }
    }
}

}