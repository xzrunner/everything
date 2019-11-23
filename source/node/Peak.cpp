#include "sop/node/Peak.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/node/Normal.h"

namespace sop
{
namespace node
{

void Peak::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    std::shared_ptr<Group> group = nullptr;
    if (!m_group_name.empty())
    {
        group = m_geo_impl->GetGroup().Query(m_group_name);
        if (!group) {
            return;
        }
    }

    if (group)
    {
        auto type = m_group_type == GroupType::GuessFromGroup ? group->GetType() : m_group_type;
        if (type != group->GetType()) {
            return;
        }

        switch (type)
        {
        case GroupType::Points:
            TranslatePoints(m_dist, group);
            break;
        case GroupType::Vertices:
            TranslateVertices(m_dist, group);
            break;
        case GroupType::Primitives:
            TranslatePrimitives(m_dist, group);
            break;
        default:
            assert(0);
        }
    }
    else
    {
        auto type = m_geo_impl->GetAdaptorType();
        switch (type)
        {
        case GeoAdaptor::Type::Shape:
            TranslatePoints(m_dist, nullptr);
            break;
        case GeoAdaptor::Type::Brush:
            TranslatePrimitives(m_dist, nullptr);
            break;
        default:
            assert(0);
        }
    }
}

void Peak::TranslatePoints(float dist, const std::shared_ptr<Group>& group)
{
    assert(m_geo_impl);
    auto& attr = m_geo_impl->GetAttr();
    auto norm_idx = attr.QueryAttrIdx(GeoAttrClass::Point, GEO_ATTR_NORM);
    if (norm_idx >= 0)
    {
        auto& pts = attr.GetPoints();
        if (group)
        {
            for (auto& i : group->GetItems()) {
                auto& p = pts[i];
                auto norm = static_cast<const sm::vec3*>(p->vars[norm_idx].p);
                p->pos += *norm * dist;
            }
        }
        else
        {
            for (auto& p : pts) {
                auto norm = static_cast<const sm::vec3*>(p->vars[norm_idx].p);
                p->pos += *norm * dist;
            }
        }

        m_geo_impl->UpdateByAttr();
    }
    else
    {
        std::vector<sm::vec3> norms;
        if (Normal::CalcSmoothedPointsNormal(*m_geo_impl, norms))
        {
            auto& pts = attr.GetPoints();
            assert(norms.size() == pts.size());
            if (group) {
                for (auto& i : group->GetItems()) {
                    pts[i]->pos += norms[i] * dist;
                }
            } else {
                for (size_t i = 0, n = pts.size(); i < n; ++i) {
                    pts[i]->pos += norms[i] * dist;
                }
            }

            m_geo_impl->UpdateByAttr();
        }
    }
}

void Peak::TranslateVertices(float dist, const std::shared_ptr<Group>& group)
{
    assert(m_geo_impl);
    auto& attr = m_geo_impl->GetAttr();
    auto norm_idx = attr.QueryAttrIdx(GeoAttrClass::Vertex, GEO_ATTR_NORM);
    if (norm_idx >= 0)
    {
        auto& vts = attr.GetVertices();
        if (group)
        {
            for (auto& i : group->GetItems()) {
                auto& v = vts[i];
                auto norm = static_cast<const sm::vec3*>(v->vars[norm_idx].p);
                v->point->pos += *norm * dist;
            }
        }
        else
        {
            for (auto& v : vts) {
                auto norm = static_cast<const sm::vec3*>(v->vars[norm_idx].p);
                v->point->pos += *norm * dist;
            }
        }

        m_geo_impl->UpdateByAttr();
    }
    else
    {
        std::vector<sm::vec3> norms;
        if (Normal::CalcSmoothedPointsNormal(*m_geo_impl, norms))
        {
            auto& vts = attr.GetVertices();
            assert(norms.size() == vts.size());
            if (group) {
                for (auto& i : group->GetItems()) {
                    auto idx = vts[i]->point->attr_idx;
                    vts[i]->point->pos += norms[idx] * dist;
                }
            } else {
                for (auto& v : vts) {
                    auto idx = v->point->attr_idx;
                    v->point->pos += norms[idx] * dist;
                }
            }

            m_geo_impl->UpdateByAttr();
        }
    }
}

void Peak::TranslatePrimitives(float dist, const std::shared_ptr<Group>& group)
{
    assert(m_geo_impl);
    auto& attr = m_geo_impl->GetAttr();
    auto& prims = attr.GetPrimtives();

    auto trans_prim = [dist](GeoAttribute::Primitive& prim)
    {
        auto norm = prim.CalcNormal();
        auto off = norm * dist;
        for (auto& v : prim.vertices) {
            v->point->pos += off;
        }
    };

    if (group) {
        for (auto& i : group->GetItems()) {
            trans_prim(*prims[i]);
        }
    } else {
        for (auto& prim : prims) {
            trans_prim(*prim);
        }
    }

    m_geo_impl->UpdateByAttr();
}

}
}