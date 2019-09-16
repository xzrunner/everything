#include "everything/node/Normal.h"
#include "everything/NodeHelper.h"
#include "everything/GeometryImpl.h"

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

void Normal::Execute(Evaluator& eval, TreeContext& ctx)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    switch (m_attr_add_norm_to)
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

void Normal::SetAttrAddNormalTo(GeoAttrType attr)
{
    if (m_attr_add_norm_to == attr) {
        return;
    }

    m_attr_add_norm_to = attr;

    SetDirty(true);
}

void Normal::AddToPoint()
{
    auto type = m_geo_impl->GetAdaptorType();
    switch (type)
    {
    case GeoAdaptor::Type::Shape:
    {
        auto& attr = m_geo_impl->GetAttr();
        auto& pts = attr.GetPoints();
        std::shared_ptr<GeoAttribute::AttrList> attrs[3];
        for (int i = 0; i < 3; ++i)
        {
            auto list = std::make_shared<GeoAttribute::AttrList>();
            for (int j = 0, m = pts.size(); j < m; ++j) {
                list->vars.push_back(Variable(0.0f));
            }
            attrs[i] = list;
        }
        attrs[0]->name = "N[x]";
        attrs[1]->name = "N[y]";
        attrs[2]->name = "N[z]";
        for (int i = 0; i < 3; ++i) {
            attr.AddAttr(GeoAttrType::Point, attrs[i]);
        }
    }
        break;
    case GeoAdaptor::Type::Brush:
    {
        auto& attr = m_geo_impl->GetAttr();

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

        std::shared_ptr<GeoAttribute::AttrList> attrs[3];
        for (int i = 0; i < 3; ++i) {
            attrs[i] = std::make_shared<GeoAttribute::AttrList>();
        }
        attrs[0]->name = "N[x]";
        attrs[1]->name = "N[y]";
        attrs[2]->name = "N[z]";
        for (int i = 0; i < 3; ++i) {
            attr.AddAttr(GeoAttrType::Point, attrs[i]);
        }
        for (int i = 0, n = norms.size(); i < n; ++i)
        {
            assert(norms_count[i] == 3);
            auto norm = norms[i] / static_cast<float>(norms_count[i]);
            for (int i = 0; i < 3; ++i) {
                attrs[i]->vars.push_back(Variable(norm.xyz[i]));
            }
        }
    }
        break;
    default:
        assert(0);
    }
}

void Normal::AddToVertex()
{
    auto type = m_geo_impl->GetAdaptorType();
    if (type == GeoAdaptor::Type::Shape) {
        return;
    }

    assert(type == GeoAdaptor::Type::Brush);

    auto& attr = m_geo_impl->GetAttr();
    std::shared_ptr<GeoAttribute::AttrList> attrs[3];
    for (int i = 0; i < 3; ++i) {
        attrs[i] = std::make_shared<GeoAttribute::AttrList>();
    }
    attrs[0]->name = "N[x]";
    attrs[1]->name = "N[y]";
    attrs[2]->name = "N[z]";
    for (int i = 0; i < 3; ++i) {
        attr.AddAttr(GeoAttrType::Point, attrs[i]);
    }

    for (auto& v : attr.GetVertices())
    {
        auto prim = v->prim.lock();
        assert(prim);
        auto norm = CalcPrimNorm(*prim);
        for (int i = 0; i < 3; ++i) {
            attrs[i]->vars.push_back(Variable(norm.xyz[i]));
        }
    }
}

void Normal::AddToPrimitive()
{
    auto type = m_geo_impl->GetAdaptorType();
    if (type == GeoAdaptor::Type::Shape) {
        return;
    }

    assert(type == GeoAdaptor::Type::Brush);

    auto& attr = m_geo_impl->GetAttr();
    std::shared_ptr<GeoAttribute::AttrList> attrs[3];
    for (int i = 0; i < 3; ++i) {
        attrs[i] = std::make_shared<GeoAttribute::AttrList>();
    }
    attrs[0]->name = "N[x]";
    attrs[1]->name = "N[y]";
    attrs[2]->name = "N[z]";
    for (int i = 0; i < 3; ++i) {
        attr.AddAttr(GeoAttrType::Point, attrs[i]);
    }

    for (auto& prim : attr.GetPrimtives())
    {
        assert(prim->vertices.size() > 2);
        auto norm = CalcPrimNorm(*prim);
        for (int i = 0; i < 3; ++i) {
            attrs[i]->vars.push_back(Variable(norm.xyz[i]));
        }
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
        auto& pts = attr.GetPoints();
        std::shared_ptr<GeoAttribute::AttrList> attrs[3];
        for (int i = 0; i < 3; ++i) {
            attrs[i] = std::make_shared<GeoAttribute::AttrList>();
            attrs[i]->vars.push_back(Variable(0.0f));
        }
        attrs[0]->name = "N[x]";
        attrs[1]->name = "N[y]";
        attrs[2]->name = "N[z]";
        for (int i = 0; i < 3; ++i) {
            attr.AddAttr(GeoAttrType::Detail, attrs[i]);
        }
    }
        break;
    case GeoAdaptor::Type::Brush:
        break;
    default:
        assert(0);
    }
}

}
}