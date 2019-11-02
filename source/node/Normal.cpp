#include "sop/node/Normal.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/GeoAttrDefine.h"

#include <SM_Calc.h>

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
    NODE_PROP_SET(m_attr_add_to, cls);
}

bool Normal::CalcSmoothedPointsNormal(const GeometryImpl& geo, std::vector<sm::vec3>& out_norms)
{
    auto& attr = geo.GetAttr();

    std::map<std::shared_ptr<GeoAttribute::Point>, size_t> point2idx;
    auto& pts = attr.GetPoints();
    for (size_t i = 0, n = pts.size(); i < n; ++i) {
        point2idx.insert({ pts[i], i });
    }

    std::vector<sm::vec3> norms(pts.size(), sm::vec3(0, 0, 0));
    std::vector<size_t>   norm_counts(pts.size(), 0);
    for (auto& prim : attr.GetPrimtives())
    {
        auto norm = CalcPrimNorm(*prim);
        for (auto& v : prim->vertices)
        {
            auto itr = point2idx.find(v->point);
            assert(itr != point2idx.end());
            norms[itr->second] += norm;
            ++norm_counts[itr->second];
        }
    }

    out_norms.resize(norms.size(), sm::vec3(0, 0, 0));
    for (int i = 0, n = norms.size(); i < n; ++i)
    {
        if (norm_counts[i] == 0)
        {
            out_norms.clear();
            return false;
        }
        else
        {
            out_norms[i] = norms[i] / static_cast<float>(norm_counts[i]);
            if (out_norms[i] == sm::vec3(0, 0, 0)) {
                out_norms.clear();
                return false;
            } else {
                out_norms[i].Normalize();
            }
        }
    }
    return true;
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
        std::vector<sm::vec3> norms;
        bool succ = CalcSmoothedPointsNormal(*m_geo_impl, norms);
        assert(succ);
        auto& attr = m_geo_impl->GetAttr();
        assert(norms.size() == attr.GetPoints().size());

        std::vector<VarValue> vars;
        vars.reserve(norms.size());
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

sm::vec3 Normal::CalcPrimNorm(const sop::GeoAttribute::Primitive& prim)
{
    auto& vts = prim.vertices;
    if (vts.size() < 3) {
        return sm::vec3(0, 0, 0);
    }

    sm::vec3 ret(0, 0, 0);
    switch (prim.type)
    {
    case sop::GeoAttribute::Primitive::Type::PolygonFace:
    {
        auto a = vts[1]->point->pos - vts[0]->point->pos;
        auto b = vts[2]->point->pos - vts[0]->point->pos;
        ret = a.Cross(b);
        if (ret != sm::vec3(0, 0, 0)) {
            ret.Normalize();
        }
    }
        break;

    case sop::GeoAttribute::Primitive::Type::PolygonCurves:
    {
        sm::vec3 norm_sum(0, 0, 0);
        size_t norm_count = 0;

        for (size_t i = 1, n = vts.size() - 1; i < n; ++i)
        {
            auto a = vts[i]->point->pos - vts[i - 1]->point->pos;
            auto b = vts[i]->point->pos - vts[i + 1]->point->pos;
            auto cross = a.Cross(b);
            if (cross != sm::vec3(0, 0, 0))
            {
                cross.Normalize();
                norm_sum += cross;
                ++norm_count;
            }
        }
        if (norm_count > 0) {
            ret = norm_sum / static_cast<float>(norm_count);
            if (ret != sm::vec3(0, 0, 0)) {
                ret.Normalize();
            }
        }
    }
        break;

    default:
        assert(0);
    }
    return ret;
}

}
}