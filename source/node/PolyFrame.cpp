#include "sop/node/PolyFrame.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/node/Normal.h"

namespace sop
{
namespace node
{

void PolyFrame::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    if (m_entity_type != GroupType::Primitives &&
        m_entity_type != GroupType::Points) {
        return;
    }

    CreateAttrs();
}

void PolyFrame::SetEntityType(GroupType type)
{
    NODE_PROP_SET(m_entity_type, type);
}

void PolyFrame::SetFrameStyle(FrameStyle style)
{
    NODE_PROP_SET(m_frame_style, style);
}

void PolyFrame::SetNormalName(const std::string& name)
{
    NODE_PROP_SET(m_normal_name, name);
}

void PolyFrame::SetTangentName(const std::string& name)
{
    NODE_PROP_SET(m_tangent_name, name);
}

void PolyFrame::SetBitangentName(const std::string& name)
{
    NODE_PROP_SET(m_bitangent_name, name);
}

void PolyFrame::CreateAttrs()
{
    // todo: only support FirstEdge and TwoEdges now
    if (m_frame_style != FrameStyle::FirstEdge &&
        m_frame_style != FrameStyle::TwoEdges) {
        return;
    }

    auto normals  = CalcPointsNormal();
    auto tangents = CalcPointsTangent();
    assert(normals.size() == tangents.size());
    std::vector<sm::vec3> bitangents;
    bitangents.resize(normals.size());
    for (size_t i = 0, n = normals.size(); i < n; ++i) {
        bitangents[i] = tangents[i].Cross(normals[i]);
        if (m_ortho) {
            tangents[i] = normals[i].Cross(bitangents[i]);
        }
    }

    if (!m_normal_name.empty()) {
        AddToPointsAttr(m_normal_name, normals);
    }
    if (!m_tangent_name.empty()) {
        AddToPointsAttr(m_tangent_name, tangents);
    }
    if (!m_bitangent_name.empty()) {
        AddToPointsAttr(m_bitangent_name, bitangents);
    }
}

std::vector<sm::vec3> 
PolyFrame::CalcPointsNormal() const
{
    std::vector<sm::vec3> normals;

    auto& attr = m_geo_impl->GetAttr();
    auto& pts = attr.GetPoints();

    // prior use attr
    const int norm_idx = attr.QueryAttrIdx(GeoAttrClass::Point, GEO_ATTR_NORM);
    if (norm_idx >= 0)
    {
        normals.resize(pts.size());
        for (size_t i = 0, n = pts.size(); i < n; ++i) {
            normals[i] = *static_cast<const sm::vec3*>(pts[i]->vars[norm_idx].p);
        }
        return normals;
    }

    normals = Normal::CalcSmoothedPointsNormal(*m_geo_impl);
    if (normals.empty()) {
        normals.resize(pts.size(), sm::vec3(0, 0, 0));
    }
    assert(normals.size() == pts.size());

    return normals;
}

std::vector<sm::vec3> 
PolyFrame::CalcPointsTangent() const
{
    switch (m_geo_impl->GetAdaptorType())
    {
    case GeoAdaptor::Type::Brush:
        return CalcBrushPointsTangent();
    case GeoAdaptor::Type::Shape:
        return CalcShapePointsTangent();
    default:
        assert(0);
        return std::vector<sm::vec3>();
    }
}

std::vector<sm::vec3> 
PolyFrame::CalcShapePointsTangent() const
{
    auto& attr = m_geo_impl->GetAttr();

    std::map<std::shared_ptr<GeoAttribute::Point>, size_t> point2idx;
    auto& pts = attr.GetPoints();
    for (size_t i = 0, n = pts.size(); i < n; ++i) {
        point2idx.insert({ pts[i], i });
    }

    std::vector<sm::vec3> tangents(pts.size(), sm::vec3(0, 0, 0));
    for (auto& prim : attr.GetPrimtives())
    {
        assert(prim->type == sop::GeoAttribute::Primitive::Type::PolygonCurves);
        auto& vts = prim->vertices;
        if (vts.size() < 2) {
            continue;
        }

        std::vector<sm::vec3> ts(vts.size(), sm::vec3(0, 0, 0));
        for (size_t i = 1, n = vts.size(); i < n; ++i) {
            ts[i] = (vts[i - 1]->point->pos - vts[i]->point->pos).Normalized();
        }
        ts[0] = ts[1];
        if (m_frame_style == FrameStyle::TwoEdges) 
        {
            auto new_ts = ts;
            for (size_t i = 1, n = vts.size() - 1; i < n; ++i) {
                new_ts[i] = ((ts[i - 1] + ts[i + 1]) / 2).Normalized();
            }
            ts = new_ts;
        }

        for (size_t i = 0, n = vts.size(); i < n; ++i)
        {
            auto itr = point2idx.find(vts[i]->point);
            assert(itr != point2idx.end());
            tangents[itr->second] = ts[i];
        }
    }

    return tangents;
}

std::vector<sm::vec3> 
PolyFrame::CalcBrushPointsTangent() const
{
    // todo
    assert(0);
    return std::vector<sm::vec3>();
}

void PolyFrame::AddToPointsAttr(const std::string& name, const std::vector<sm::vec3>& val)
{
    auto& attr = m_geo_impl->GetAttr();
    auto& pts = attr.GetPoints();

    std::vector<VarValue> vars;
    assert(val.size() == pts.size());
    vars.reserve(val.size());
    for (auto& v : val) {
        vars.push_back(VarValue(v));
    }

    GeoAttribute::VarDesc desc(name, GeoAttrType::Vector);
    attr.AddAttr(GeoAttrClass::Point, desc, vars);
}

}
}