#include "sop/node/Normal.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/GeoAttrDefine.h"

#include <SM_Calc.h>

namespace
{

sm::vec3 CalcPrimNorm(const sop::GeoAttribute::Primitive& prim)
{
    switch (prim.type)
    {
    case sop::GeoAttribute::Primitive::Type::PolygonFace:
    {
        assert(prim.vertices.size() > 2);
        auto a = prim.vertices[1]->point->pos - prim.vertices[0]->point->pos;
        auto b = prim.vertices[2]->point->pos - prim.vertices[0]->point->pos;
        auto norm = a.Cross(b);
        norm.Normalize();
        return norm;
    }
    case sop::GeoAttribute::Primitive::Type::PolygonCurves:
        return sm::vec3(0, 0, 0);
    default:
        assert(0);
        return sm::vec3(0, 0, 0);
    }
}

}

namespace sop
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
    case GeoAttrClass::Point:
        AddToPoint();
        break;
    case GeoAttrClass::Vertex:
        AddToVertex();
        break;
    case GeoAttrClass::Primitive:
        AddToPrimitive();
        break;
    case GeoAttrClass::Detail:
        AddToDetail();
        break;
    default:
        assert(0);
    }
}

void Normal::SetAttrAddTo(GeoAttrClass cls)
{
    if (m_attr_add_to == cls) {
        return;
    }

    m_attr_add_to = cls;

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
        std::vector<VarValue> vars(attr.GetPoints().size(), VarValue(sm::vec3(0, 0, 0)));
        attr.AddAttr(GeoAttrClass::Point, GEO_ATTR_NORM, vars);
    }
        break;
    case GeoAdaptor::Type::Brush:
    {
        auto norms = CalcBrushPointsNormal(*m_geo_impl);
        auto& attr = m_geo_impl->GetAttr();
        assert(norms.size() == attr.GetPoints().size());

        std::vector<VarValue> vars;
        for (auto& norm : norms) {
            vars.push_back(VarValue(norm));
        }
        attr.AddAttr(GeoAttrClass::Point, GEO_ATTR_NORM, vars);
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
        std::vector<VarValue> vars(attr.GetVertices().size(), VarValue(sm::vec3(0, 0, 0)));
        attr.AddAttr(GeoAttrClass::Vertex, GEO_ATTR_NORM, vars);
    }
        break;
    case GeoAdaptor::Type::Brush:
    {
        auto& attr = m_geo_impl->GetAttr();
        auto& vts = attr.GetVertices();
        std::vector<VarValue> vars;
        for (auto& v : vts)
        {
            auto prim = v->prim.lock();
            assert(prim);
            auto norm = CalcPrimNorm(*prim);
            vars.push_back(VarValue(norm));
        }
        attr.AddAttr(GeoAttrClass::Vertex, GEO_ATTR_NORM, vars);
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
        std::vector<VarValue> vars(attr.GetPrimtives().size(), VarValue(sm::vec3(0, 0, 0)));
        attr.AddAttr(GeoAttrClass::Primitive, GEO_ATTR_NORM, vars);
    }
        break;
    case GeoAdaptor::Type::Brush:
    {
        auto& attr = m_geo_impl->GetAttr();
        auto& prims = attr.GetPrimtives();
        std::vector<VarValue> vars;
        for (auto& prim : prims)
        {
            assert(prim->vertices.size() > 2);
            auto norm = CalcPrimNorm(*prim);
            vars.push_back(VarValue(norm));
        }
        attr.AddAttr(GeoAttrClass::Primitive, GEO_ATTR_NORM, vars);
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
        std::vector<VarValue> vars(1, VarValue(sm::vec3(0, 0, 0)));
        attr.AddAttr(GeoAttrClass::Detail, GEO_ATTR_NORM, vars);
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

        std::vector<VarValue> vars(1, VarValue(norm));
        attr.AddAttr(GeoAttrClass::Detail, GEO_ATTR_NORM, vars);
    }
        break;
    default:
        assert(0);
    }
}

}
}