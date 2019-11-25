#include "sop/node/Normal.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/GeoAttrDefine.h"
#include "sop/ParmList.h"

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
    case AddToType::Points:
        AddToPoint();
        break;
    case AddToType::Vertices:
        AddToVertex();
        break;
    case AddToType::Primitives:
        AddToPrimitive();
        break;
    case AddToType::Detail:
        AddToDetail();
        break;
    default:
        assert(0);
    }
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
        std::vector<sm::vec3> data(attr.GetPoints().size(), sm::vec3(0, 0, 0));
        attr.AddParmList(GeoAttrClass::Point,
            std::make_shared<ParmFlt3List>(GEO_ATTR_NORM, data)
        );
    }
        break;
    case GeoAdaptor::Type::Brush:
    {
        std::vector<sm::vec3> norms;
        bool succ = CalcSmoothedPointsNormal(*m_geo_impl, norms);
        assert(succ);
        auto& attr = m_geo_impl->GetAttr();
        assert(norms.size() == attr.GetPoints().size());

        attr.AddParmList(GeoAttrClass::Point,
            std::make_shared<ParmFlt3List>(GEO_ATTR_NORM, norms)
        );
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
        std::vector<sm::vec3> data(attr.GetVertices().size(), sm::vec3(0, 0, 0));
        attr.AddParmList(GeoAttrClass::Vertex,
            std::make_shared<ParmFlt3List>(GEO_ATTR_NORM, data)
        );
    }
        break;
    case GeoAdaptor::Type::Brush:
    {
        auto& attr = m_geo_impl->GetAttr();
        auto& vts = attr.GetVertices();
        std::vector<sm::vec3> data;
        data.reserve(vts.size());
        for (auto& v : vts)
        {
            auto prim = v->prim.lock();
            assert(prim);
            auto norm = CalcPrimNorm(*prim);
            data.push_back(norm);
        }
        attr.AddParmList(GeoAttrClass::Vertex,
            std::make_shared<ParmFlt3List>(GEO_ATTR_NORM, data)
        );
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
        std::vector<sm::vec3> data(attr.GetPrimtives().size(), sm::vec3(0, 0, 0));
        attr.AddParmList(GeoAttrClass::Primitive,
            std::make_shared<ParmFlt3List>(GEO_ATTR_NORM, data)
        );
    }
        break;
    case GeoAdaptor::Type::Brush:
    {
        auto& attr = m_geo_impl->GetAttr();
        auto& prims = attr.GetPrimtives();
        std::vector<sm::vec3> data;
        data.reserve(prims.size());
        for (auto& prim : prims)
        {
            assert(prim->vertices.size() > 2);
            auto norm = CalcPrimNorm(*prim);
            data.push_back(norm);
        }
        attr.AddParmList(GeoAttrClass::Primitive,
            std::make_shared<ParmFlt3List>(GEO_ATTR_NORM, data)
        );
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
        std::vector<sm::vec3> data(1, sm::vec3(0, 0, 0));
        attr.AddParmList(GeoAttrClass::Detail,
            std::make_shared<ParmFlt3List>(GEO_ATTR_NORM, data)
        );
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

        std::vector<sm::vec3> data(1, norm);
        attr.AddParmList(GeoAttrClass::Detail,
            std::make_shared<ParmFlt3List>(GEO_ATTR_NORM, data)
        );
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