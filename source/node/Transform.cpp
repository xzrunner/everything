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

    auto mat = CalcTransformMat();
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

void Transform::SetTranslate(const sm::vec3& t)
{
    if (m_props.SetValue(TRANS, Variable(t))) {
        SetDirty(true);
    }
}

void Transform::SetRotate(const sm::vec3& r)
{
    if (m_props.SetValue(ROT, Variable(r))) {
        SetDirty(true);
    }
}

void Transform::SetScale(const sm::vec3& s)
{
    if (m_props.SetValue(SCALE, Variable(s))) {
        SetDirty(true);
    }
}

void Transform::SetShear(const sm::vec3& s)
{
    if (m_props.SetValue(SHEAR, Variable(s))) {
        SetDirty(true);
    }
}

void Transform::SetGroupName(const std::string& name)
{
    NODE_PROP_SET(m_group_name, name);
}

void Transform::SetGroupType(GroupType type)
{
    NODE_PROP_SET(m_group_type, type);
}

sm::mat4 Transform::CalcTransformMat(const sm::vec3& translate, const sm::vec3& rotate,
                                     const sm::vec3& scale, const sm::vec3& shear)
{
    auto s_mat = sm::mat4::Scaled(scale.x, scale.y, scale.z);
    auto r_mat = sm::mat4::Rotated(rotate.x, rotate.y, rotate.z);
    auto t_mat = sm::mat4::Translated(translate.x, translate.y, translate.z);
    return t_mat * r_mat * s_mat;
}

void Transform::InitProps()
{
    m_props.Assign(TRANS, PropNames[TRANS], Variable(sm::vec3(0.0f, 0.0f, 0.0f)));
    m_props.Assign(ROT,   PropNames[ROT],   Variable(sm::vec3(0.0f, 0.0f, 0.0f)));
    m_props.Assign(SCALE, PropNames[SCALE], Variable(sm::vec3(1.0f, 1.0f, 1.0f)));
    m_props.Assign(SHEAR, PropNames[SHEAR], Variable(sm::vec3(0.0f, 0.0f, 0.0f)));
}

sm::mat4 Transform::CalcTransformMat() const
{
    auto& props = m_props.GetProps();

    assert(props[SCALE].Val().type == VarType::Float3);
    auto scale = static_cast<const sm::vec3*>(props[SCALE].Val().p);

    assert(props[ROT].Val().type == VarType::Float3);
    auto rotate = static_cast<const sm::vec3*>(props[ROT].Val().p);

    assert(props[TRANS].Val().type == VarType::Float3);
    auto translate = static_cast<const sm::vec3*>(props[TRANS].Val().p);

    return CalcTransformMat(*translate, *rotate * SM_DEG_TO_RAD, *scale, sm::vec3(0, 0, 0));
}

}
}