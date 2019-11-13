#include "sop/node/Primitive.h"
#include "sop/node/Transform.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

namespace sop
{
namespace node
{

void Primitive::Execute(Evaluator& eval)
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

    auto& attr = m_geo_impl->GetAttr();
    if (group)
    {
        sm::vec3 c;
        switch (group->GetType())
        {
        case GroupType::Points:
        {
            auto c = m_geo_impl->GetAttr().GetAABB().Center();
            for (auto& i : group->GetItems()) {
                auto& p = attr.GetPoints()[i];
                p->pos = mat * (p->pos - c) + c;
            }
        }
            break;
        case GroupType::Primitives:
        {
            for (auto& i : group->GetItems()) {
                UpdatePrim(*attr.GetPrimtives()[i], mat);
            }
        }
            break;
        default:
            assert(0);
        }
    }
    else
    {
        for (auto& prim : m_geo_impl->GetAttr().GetPrimtives()) {
            UpdatePrim(*prim, mat);
        }
    }

    m_geo_impl->UpdateByAttr();
}

void Primitive::SetGroupName(const std::string& name)
{
    NODE_PROP_SET(m_group_name, name);
}

void Primitive::SetTranslate(const sm::vec3& t)
{
    if (m_props.SetValue(TRANS, Variable(t))) {
        SetDirty(true);
    }
}

void Primitive::SetRotate(const sm::vec3& r)
{
    if (m_props.SetValue(ROT, Variable(r))) {
        SetDirty(true);
    }
}

void Primitive::SetScale(const sm::vec3& s)
{
    if (m_props.SetValue(SCALE, Variable(s))) {
        SetDirty(true);
    }
}

void Primitive::SetShear(const sm::vec3& s)
{
    if (m_props.SetValue(SHEAR, Variable(s))) {
        SetDirty(true);
    }
}

void Primitive::InitProps()
{
    m_props.Assign(TRANS, PropNames[TRANS], Variable(sm::vec3(0.0f, 0.0f, 0.0f)));
    m_props.Assign(ROT,   PropNames[ROT],   Variable(sm::vec3(0.0f, 0.0f, 0.0f)));
    m_props.Assign(SCALE, PropNames[SCALE], Variable(sm::vec3(1.0f, 1.0f, 1.0f)));
    m_props.Assign(SHEAR, PropNames[SHEAR], Variable(sm::vec3(0.0f, 0.0f, 0.0f)));
}

void Primitive::UpdatePrim(GeoAttribute::Primitive& prim, const sm::mat4& mat)
{
    sm::vec3 c(0, 0, 0);
    assert(!prim.vertices.empty());
    for (auto& v : prim.vertices) {
        c += v->point->pos;
    }
    c /= static_cast<float>(prim.vertices.size());

    for (auto& v : prim.vertices) {
        v->point->pos = mat * (v->point->pos - c) + c;
    }
}

sm::mat4 Primitive::CalcTransformMat() const
{
    auto& props = m_props.GetProps();

    assert(props[SCALE].Val().type == VarType::Float3);
    auto scale = static_cast<const sm::vec3*>(props[SCALE].Val().p);

    assert(props[ROT].Val().type == VarType::Float3);
    auto rotate = static_cast<const sm::vec3*>(props[ROT].Val().p);

    assert(props[TRANS].Val().type == VarType::Float3);
    auto translate = static_cast<const sm::vec3*>(props[TRANS].Val().p);

    return Transform::CalcTransformMat(*translate, *rotate * SM_DEG_TO_RAD, *scale, sm::vec3(0, 0, 0));
}

}
}