#include "sop/node/Transform.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

namespace sop
{
namespace node
{

void Transform::Execute(Evaluator& eval)
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

    auto mat = CalcTransformMat(m_translate, m_rotate * SM_DEG_TO_RAD, m_scale, sm::vec3(0, 0, 0));
    if (group)
    {
        auto type = m_group_type == GroupType::GuessFromGroup ? group->GetType() : m_group_type;
        if (type != group->GetType()) {
            return;
        }

        switch (type)
        {
        case GroupType::Points:
        {
            auto& pts = m_geo_impl->GetAttr().GetPoints();
            for (auto i : group->GetItems()) {
                pts[i]->pos = mat * pts[i]->pos;
            }
        }
            break;

        case GroupType::Vertices:
        {
            std::set<std::shared_ptr<GeoAttribute::Point>> pts;
            auto& vts = m_geo_impl->GetAttr().GetVertices();
            for (auto i : group->GetItems()) {
                pts.insert(vts[i]->point);
            }
            for (auto p : pts) {
                p->pos = mat * p->pos;
            }
        }
            break;

        case GroupType::Primitives:
        {
            std::set<std::shared_ptr<GeoAttribute::Point>> pts;
            auto& prims = m_geo_impl->GetAttr().GetPrimtives();
            for (auto i : group->GetItems()) {
                for (auto v : prims[i]->vertices) {
                    pts.insert(v->point);
                }
            }
            for (auto p : pts) {
                p->pos = mat * p->pos;
            }
        }
            break;

        default:
            assert(0);
        }
    }
    else
    {
        auto& attr = m_geo_impl->GetAttr();
        for (auto& p : attr.GetPoints()) {
            p->pos = mat * p->pos;
        }
    }
    m_geo_impl->UpdateByAttr();
}

sm::mat4 Transform::CalcTransformMat(const sm::vec3& translate, const sm::vec3& rotate,
                                     const sm::vec3& scale, const sm::vec3& shear)
{
    auto s_mat = sm::mat4::Scaled(scale.x, scale.y, scale.z);
    auto r_mat = sm::mat4::Rotated(rotate.x, rotate.y, rotate.z);
    auto t_mat = sm::mat4::Translated(translate.x, translate.y, translate.z);
    return t_mat * r_mat * s_mat;
}

}
}