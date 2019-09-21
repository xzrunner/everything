#include "everything/node/Normal.h"
#include "everything/NodeHelper.h"
#include "everything/GeometryImpl.h"
#include "everything/GeoAttrName.h"

#include <SM_Calc.h>

namespace
{

sm::vec3 CalcPrimNorm(const evt::GeoAttribute::Primitive& prim)
{
    switch (prim.type)
    {
    case evt::GeoAttribute::Primitive::Type::PolygonFace:
    {
        assert(prim.vertices.size() > 2);
        auto a = prim.vertices[1]->point->pos - prim.vertices[0]->point->pos;
        auto b = prim.vertices[2]->point->pos - prim.vertices[0]->point->pos;
        auto norm = a.Cross(b);
        norm.Normalize();
        return norm;
    }
    case evt::GeoAttribute::Primitive::Type::PolygonCurves:
        return sm::vec3(0, 0, 0);
    default:
        assert(0);
        return sm::vec3(0, 0, 0);
    }
}

}

namespace evt
{
namespace node
{

void Normal::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    switch (m_attr_add_to)
    {
    case GeoAttrType::Point:
        AddToPoint();
        break;
    case GeoAttrType::Vertex:
        AddToVertex();
        break;
    case GeoAttrType::Primitive:
        AddToPrimitive();
        break;
    case GeoAttrType::Detail:
        AddToDetail();
        break;
    default:
        assert(0);
    }
}

void Normal::SetAttrAddTo(GeoAttrType attr)
{
    if (m_attr_add_to == attr) {
        return;
    }

    m_attr_add_to = attr;

    SetDirty(true);
}

std::vector<sm::vec3> Normal::CalcBrushPointsNormal(const GeometryImpl& geo)
{
    if (geo.GetAdaptorType() != GeoAdaptor::Type::Brush) {
        return std::vector<sm::vec3>();
    }

    auto& attr = geo.GetAttr();

    std::map<std::shared_ptr<GeoAttribute::Point>, size_t> point2idx;
    auto& pts = attr.GetPoints();
    for (size_t i = 0, n = pts.size(); i < n; ++i) {
        point2idx.insert({ pts[i], i });
    }

    std::vector<sm::vec3> norms(pts.size(), sm::vec3(0, 0, 0));
    std::vector<size_t>   norms_count(pts.size(), 0);

    for (auto& prim : attr.GetPrimtives())
    {
        auto norm = CalcPrimNorm(*prim);
        for (auto& v : prim->vertices)
        {
            auto itr = point2idx.find(v->point);
            assert(itr != point2idx.end());
            norms[itr->second] += norm;
            ++norms_count[itr->second];
        }
    }

    std::vector<sm::vec3> ret(norms.size(), sm::vec3(0, 0, 1));
    for (int i = 0, n = norms.size(); i < n; ++i) {
        assert(norms_count[i] > 0);
        ret[i] = norms[i] / static_cast<float>(norms_count[i]);
    }
    return ret;
}

void Normal::AddToPoint()
{
    auto type = m_geo_impl->GetAdaptorType();
    switch (type)
    {
    case GeoAdaptor::Type::Shape:
    {
        auto& attr = m_geo_impl->GetAttr();
        std::vector<VarValue> zero_list(attr.GetPoints().size(), VarValue(0.0f));
        attr.AddAttr(GeoAttrType::Point, { GeoAttrName::norm_x, VarType::Float }, zero_list);
        attr.AddAttr(GeoAttrType::Point, { GeoAttrName::norm_y, VarType::Float }, zero_list);
        attr.AddAttr(GeoAttrType::Point, { GeoAttrName::norm_z, VarType::Float }, zero_list);
    }
        break;
    case GeoAdaptor::Type::Brush:
    {
        auto norms = CalcBrushPointsNormal(*m_geo_impl);
        auto& attr = m_geo_impl->GetAttr();
        assert(norms.size() == attr.GetPoints().size());

        std::vector<VarValue> x_list(norms.size(), VarValue(0.0f));
        std::vector<VarValue> y_list(norms.size(), VarValue(0.0f));
        std::vector<VarValue> z_list(norms.size(), VarValue(0.0f));
        for (int i = 0, n = norms.size(); i < n; ++i)
        {
            x_list[i] = VarValue(norms[i].x);
            y_list[i] = VarValue(norms[i].y);
            z_list[i] = VarValue(norms[i].z);
        }
        attr.AddAttr(GeoAttrType::Point, { GeoAttrName::norm_x, VarType::Float }, x_list);
        attr.AddAttr(GeoAttrType::Point, { GeoAttrName::norm_y, VarType::Float }, y_list);
        attr.AddAttr(GeoAttrType::Point, { GeoAttrName::norm_z, VarType::Float }, z_list);
    }
        break;
    default:
        assert(0);
    }
}

void Normal::AddToVertex()
{
    auto type = m_geo_impl->GetAdaptorType();
    switch (type)
    {
    case GeoAdaptor::Type::Shape:
    {
        auto& attr = m_geo_impl->GetAttr();
        std::vector<VarValue> zero_list(attr.GetVertices().size(), VarValue(0.0f));
        attr.AddAttr(GeoAttrType::Vertex, { GeoAttrName::norm_x, VarType::Float }, zero_list);
        attr.AddAttr(GeoAttrType::Vertex, { GeoAttrName::norm_y, VarType::Float }, zero_list);
        attr.AddAttr(GeoAttrType::Vertex, { GeoAttrName::norm_z, VarType::Float }, zero_list);
    }
        break;
    case GeoAdaptor::Type::Brush:
    {
        auto& attr = m_geo_impl->GetAttr();
        auto& vts = attr.GetVertices();
        std::vector<VarValue> x_list(vts.size(), VarValue(0.0f));
        std::vector<VarValue> y_list(vts.size(), VarValue(0.0f));
        std::vector<VarValue> z_list(vts.size(), VarValue(0.0f));
        for (int i = 0, n = vts.size(); i < n; ++i)
        {
            auto prim = vts[i]->prim.lock();
            assert(prim);
            auto norm = CalcPrimNorm(*prim);
            x_list[i] = VarValue(norm.x);
            y_list[i] = VarValue(norm.y);
            z_list[i] = VarValue(norm.z);
        }
        attr.AddAttr(GeoAttrType::Vertex, { GeoAttrName::norm_x, VarType::Float }, x_list);
        attr.AddAttr(GeoAttrType::Vertex, { GeoAttrName::norm_y, VarType::Float }, y_list);
        attr.AddAttr(GeoAttrType::Vertex, { GeoAttrName::norm_z, VarType::Float }, z_list);
    }
        break;
    default:
        assert(0);
    }
}

void Normal::AddToPrimitive()
{
    auto type = m_geo_impl->GetAdaptorType();
    switch (type)
    {
    case GeoAdaptor::Type::Shape:
    {
        auto& attr = m_geo_impl->GetAttr();
        std::vector<VarValue> zero_list(attr.GetPrimtives().size(), VarValue(0.0f));
        attr.AddAttr(GeoAttrType::Primitive, { GeoAttrName::norm_x, VarType::Float }, zero_list);
        attr.AddAttr(GeoAttrType::Primitive, { GeoAttrName::norm_y, VarType::Float }, zero_list);
        attr.AddAttr(GeoAttrType::Primitive, { GeoAttrName::norm_z, VarType::Float }, zero_list);
    }
        break;
    case GeoAdaptor::Type::Brush:
    {
        auto& attr = m_geo_impl->GetAttr();
        auto& prims = attr.GetPrimtives();
        std::vector<VarValue> x_list(prims.size(), VarValue(0.0f));
        std::vector<VarValue> y_list(prims.size(), VarValue(0.0f));
        std::vector<VarValue> z_list(prims.size(), VarValue(0.0f));
        for (int i = 0, n = prims.size(); i < n; ++i)
        {
            auto& prim = prims[i];
            assert(prim->vertices.size() > 2);
            auto norm = CalcPrimNorm(*prim);
            x_list[i] = VarValue(norm.x);
            y_list[i] = VarValue(norm.y);
            z_list[i] = VarValue(norm.z);
        }
        attr.AddAttr(GeoAttrType::Primitive, { GeoAttrName::norm_x, VarType::Float }, x_list);
        attr.AddAttr(GeoAttrType::Primitive, { GeoAttrName::norm_y, VarType::Float }, y_list);
        attr.AddAttr(GeoAttrType::Primitive, { GeoAttrName::norm_z, VarType::Float }, z_list);
    }
        break;
    default:
        assert(0);
    }
}

void Normal::AddToDetail()
{
    auto type = m_geo_impl->GetAdaptorType();
    switch (type)
    {
    case GeoAdaptor::Type::Shape:
    {
        auto& attr = m_geo_impl->GetAttr();
        std::vector<VarValue> zero_list(1, VarValue(0.0f));
        attr.AddAttr(GeoAttrType::Detail, { GeoAttrName::norm_x, VarType::Float }, zero_list);
        attr.AddAttr(GeoAttrType::Detail, { GeoAttrName::norm_y, VarType::Float }, zero_list);
        attr.AddAttr(GeoAttrType::Detail, { GeoAttrName::norm_z, VarType::Float }, zero_list);
    }
        break;
    case GeoAdaptor::Type::Brush:
    {
        auto& attr = m_geo_impl->GetAttr();

        sm::vec3 norm(0, 0, 0);
        auto& prims = attr.GetPrimtives();
        if (prims.size() == 1) {
            norm = CalcPrimNorm(*prims[0]);
        }

        std::vector<VarValue> x_list(1, VarValue(norm.x));
        std::vector<VarValue> y_list(1, VarValue(norm.y));
        std::vector<VarValue> z_list(1, VarValue(norm.z));
        attr.AddAttr(GeoAttrType::Detail, { GeoAttrName::norm_x, VarType::Float }, x_list);
        attr.AddAttr(GeoAttrType::Detail, { GeoAttrName::norm_y, VarType::Float }, y_list);
        attr.AddAttr(GeoAttrType::Detail, { GeoAttrName::norm_z, VarType::Float }, z_list);
    }
        break;
    default:
        assert(0);
    }
}

}
}