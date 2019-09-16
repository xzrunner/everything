#include "everything/GeoAttribute.h"
#include "everything/GeoShape.h"

#include <map>
#include <set>

namespace evt
{

GeoAttribute::GeoAttribute(const std::vector<std::shared_ptr<GeoShape>>& shapes)
{
    FromGeoShapes(shapes);
}

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
        m_vertices.push_back(std::make_shared<Vertex>());
    }

    // primitives
    m_primtives.reserve(attr.m_primtives.size());
    for (auto& prim : attr.m_primtives) {
        m_primtives.push_back(std::make_shared<Primitive>(prim->type));
    }

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

    // attr list
    for (int i = 0, n = static_cast<int>(GeoAttrType::MaxTypeNum); i < n; ++i) {
        m_attr_mgr[i] = attr.m_attr_mgr[i];
    }

    SetupAABB();

    return *this;
}

void GeoAttribute::SetPoints(const std::vector<std::shared_ptr<Point>>& points)
{
    if (m_points == points) {
        return;
    }

    m_points = points;

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

    bool prim_dirty = false;
    if (v_dirty)
    {
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
    }

    if (prim_dirty)
    {
        std::set<std::shared_ptr<Primitive>> prim_set;
        for (auto prim : m_primtives) {
            prim_set.insert(prim);
        }
        for (auto itr = m_vertices.begin(); itr != m_vertices.end(); ) {
            if (prim_set.find((*itr)->prim.lock()) == prim_set.end()) {
                itr = m_vertices.erase(itr);
            } else {
                ++itr;
            }
        }
    }

    SetupAABB();
}

void GeoAttribute::SetVertices(const std::vector<std::shared_ptr<Vertex>>& vertices)
{
    if (m_vertices == vertices) {
        return;
    }

    m_vertices = vertices;

    std::set<std::shared_ptr<Vertex>> v_set;
    std::set<std::shared_ptr<Point>>  p_set;
    for (auto v : m_vertices) {
        v_set.insert(v);
        p_set.insert(v->point);
    }

    bool p_dirty = false;
    for (auto itr = m_points.begin(); itr != m_points.end(); )
    {
        if (p_set.find(*itr) == p_set.end()) {
            p_dirty = true;
            itr = m_points.erase(itr);
        } else {
            ++itr;
        }
    }

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
            itr = m_primtives.erase(itr);
        } else {
            ++itr;
        }
    }

    if (p_dirty) {
        SetupAABB();
    }
}

void GeoAttribute::SetPrimtives(const std::vector<std::shared_ptr<Primitive>>& prims)
{
    if (m_primtives == prims) {
        return;
    }

    m_primtives = prims;

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

    if (v_dirty)
    {
        std::set<std::shared_ptr<Point>> p_set;
        for (auto v : m_vertices) {
            p_set.insert(v->point);
        }

        bool p_dirty = false;
        for (auto itr = m_points.begin(); itr != m_points.end(); ) {
            if (p_set.find(*itr) == p_set.end()) {
                p_dirty = true;
                itr = m_points.erase(itr);
            } else {
                ++itr;
            }
        }
        if (p_dirty) {
            SetupAABB();
        }
    }
}

void GeoAttribute::Reset(const std::vector<std::shared_ptr<Point>>& points,
                           const std::vector<std::shared_ptr<Vertex>>& vertices,
                           const std::vector<std::shared_ptr<Primitive>>& prims)
{
    Clear();

    m_points    = points;
    m_vertices  = vertices;
    m_primtives = prims;

    SetupAABB();
}

void GeoAttribute::AddAttr(GeoAttrType type, const std::shared_ptr<AttrList>& attr)
{
    assert(type >= static_cast<GeoAttrType>(0)
        && type < GeoAttrType::MaxTypeNum);
    auto& attrs = m_attr_mgr[static_cast<int>(type)];
    for (auto itr = attrs.attrs.begin(); itr != attrs.attrs.end(); ) {
        if ((*itr)->name == attr->name) {
            itr = attrs.attrs.erase(itr);
        } else {
            ++itr;
        }
    }
    attrs.attrs.push_back(attr);
}

std::shared_ptr<GeoAttribute::AttrList>
GeoAttribute::QueryAttr(GeoAttrType type, const std::string& name) const
{
    assert(type >= static_cast<GeoAttrType>(0)
        && type < GeoAttrType::MaxTypeNum);
    return m_attr_mgr[static_cast<int>(type)].QueryAttr(name);
}

const std::vector<std::shared_ptr<GeoAttribute::AttrList>>&
GeoAttribute::GetAttrs(GeoAttrType type) const
{
    assert(type >= static_cast<GeoAttrType>(0)
        && type < GeoAttrType::MaxTypeNum);
    return m_attr_mgr[static_cast<int>(type)].attrs;
}

void GeoAttribute::Combine(const GeoAttribute& attr)
{
    std::copy(attr.m_points.begin(),    attr.m_points.end(),    std::back_inserter(m_points));
    std::copy(attr.m_vertices.begin(),  attr.m_vertices.end(),  std::back_inserter(m_vertices));
    std::copy(attr.m_primtives.begin(), attr.m_primtives.end(), std::back_inserter(m_primtives));

    for (size_t i = 0, n = static_cast<int>(GeoAttrType::MaxTypeNum); i < n; ++i)
    {
        size_t tot_num = 0;
        switch (static_cast<GeoAttrType>(i))
        {
        case GeoAttrType::Point:
            tot_num = m_points.size();
            break;
        case GeoAttrType::Vertex:
            tot_num = m_vertices.size();
            break;
        case GeoAttrType::Primitive:
            tot_num = m_primtives.size();
            break;
        case GeoAttrType::Detail:
            tot_num = 1;
            break;
        }

        auto& src_attrs = attr.m_attr_mgr[i];
        auto& dst_attrs = m_attr_mgr[i];
        for (auto& s_attr : src_attrs.attrs)
        {
            if (s_attr->vars.empty()) {
                continue;
            }
            auto d_attr = dst_attrs.QueryAttr(s_attr->name);
            if (d_attr)
            {
                d_attr->Append(*s_attr);

                if (d_attr->vars.size() != tot_num) {
                    assert(tot_num > d_attr->vars.size());
                    assert(!d_attr->vars.empty());
                    d_attr->Append(d_attr->vars.front().type, tot_num - d_attr->vars.size());
                }
            }
            else
            {
                auto d_attr = std::make_shared<AttrList>();
                d_attr->name = s_attr->name;
                d_attr->Append(s_attr->vars.front().type, tot_num - s_attr->vars.size());
                d_attr->Append(*s_attr);
                dst_attrs.attrs.push_back(d_attr);
            }
        }
    }
}

void GeoAttribute::FromGeoShapes(const std::vector<std::shared_ptr<GeoShape>>& shapes)
{
    Clear();

    for (auto& shape : shapes)
    {
        switch (shape->Type())
        {
        case GeoShapeType::Point:
        {
            auto& vertex = static_cast<GeoPoint*>(shape.get())->GetVertex();
            m_points.push_back(std::make_shared<GeoAttribute::Point>(vertex));
        }
            break;
        case GeoShapeType::Polyline:
        {
            auto& vertices = static_cast<GeoPolyline*>(shape.get())->GetVertices();
            if (vertices.size() < 2) {
                return;
            }
            auto from_polyline = [&](const std::vector<sm::vec3>& vertices)
            {
                m_points.reserve(vertices.size());
                auto prim = std::make_shared<GeoAttribute::Primitive>(GeoAttribute::Primitive::Type::PolygonCurves);
                prim->vertices.reserve(vertices.size());
                for (auto& v : vertices)
                {
                    auto dst_p = std::make_shared<GeoAttribute::Point>(v);
                    m_points.push_back(dst_p);

                    auto dst_v = std::make_shared<GeoAttribute::Vertex>();
                    dst_v->point = dst_p;
                    dst_v->prim = prim;
                    m_vertices.push_back(dst_v);

                    prim->vertices.push_back(dst_v);
                }
                m_primtives.push_back(prim);
            };
            if (vertices.front() == vertices.back()) {
                auto del_back = vertices;
                del_back.pop_back();
                from_polyline(del_back);
            } else {
                from_polyline(vertices);
            }
        }
            break;
        }
    }

    SetupAABB();
}

void GeoAttribute::Clear()
{
    m_points.clear();
    m_vertices.clear();
    m_primtives.clear();

    for (auto& attrs : m_attr_mgr) {
        attrs.Clear();
    }

    m_aabb.MakeEmpty();
}

void GeoAttribute::SetupAABB()
{
    m_aabb.MakeEmpty();
    for (auto& p : m_points) {
        m_aabb.Combine(p->pos);
    }
}

//////////////////////////////////////////////////////////////////////////
// struct GeoAttribute::AttrList
//////////////////////////////////////////////////////////////////////////

void GeoAttribute::AttrList::
Append(VariableType type, size_t count)
{
    if (count == 0) {
        return;
    }

    Variable var;
    switch (type)
    {
    case VariableType::Bool:
        var = Variable(false);
        break;
    case VariableType::Int:
        var = Variable(0);
        break;
    case VariableType::Float:
        var = Variable(0.0f);
        break;
    case VariableType::Float3:
        var = Variable(sm::vec3());
        break;
    case VariableType::Double:
        var = Variable(0.0);
        break;
    case VariableType::String:
        var = Variable("");
        break;
    default:
        assert(0);
    }

    for (size_t i = 0; i < count; ++i) {
        vars.push_back(var);
    }
}

void GeoAttribute::AttrList::
Append(const AttrList& list)
{
    if (list.vars.empty()) {
        return;
    }

    assert(list.name == name);
    assert(vars.empty() || !vars.empty() && vars.front().type == list.vars.front().type);
    std::copy(list.vars.begin(), list.vars.end(), std::back_inserter(vars));
}

//////////////////////////////////////////////////////////////////////////
// struct GeoAttribute::AttrMgr
//////////////////////////////////////////////////////////////////////////

GeoAttribute::AttrMgr::
AttrMgr(const GeoAttribute::AttrMgr& attrs)
{
    operator = (attrs);
}

GeoAttribute::AttrMgr& GeoAttribute::AttrMgr::
operator = (const GeoAttribute::AttrMgr& src)
{
    attrs.resize(src.attrs.size());
    for (int i = 0, n = src.attrs.size(); i < n; ++i) {
        attrs[i] = std::make_shared<AttrList>(*src.attrs[i]);
    }
    return *this;
}

void GeoAttribute::AttrMgr::Clear()
{
    attrs.clear();
}

std::shared_ptr<GeoAttribute::AttrList>
GeoAttribute::AttrMgr::QueryAttr(const std::string& name) const
{
    for (auto& attr : attrs) {
        if (attr->name == name) {
            return attr;
        }
    }
    return nullptr;
}

}