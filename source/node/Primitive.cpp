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
    bool dirty = false;

    if (m_props.SetValue(TRANS_X, Variable(t.x))) {
        dirty = true;
    }
    if (m_props.SetValue(TRANS_Y, Variable(t.y))) {
        dirty = true;
    }
    if (m_props.SetValue(TRANS_Z, Variable(t.z))) {
        dirty = true;
    }

    if (dirty) {
        SetDirty(true);
    }
}

void Primitive::SetRotate(const sm::vec3& r)
{
    bool dirty = false;

    if (m_props.SetValue(ROT_X, Variable(r.x))) {
        dirty = true;
    }
    if (m_props.SetValue(ROT_Y, Variable(r.y))) {
        dirty = true;
    }
    if (m_props.SetValue(ROT_Z, Variable(r.z))) {
        dirty = true;
    }

    if (dirty) {
        SetDirty(true);
    }
}

void Primitive::SetScale(const sm::vec3& s)
{
    bool dirty = false;

    if (m_props.SetValue(SCALE_X, Variable(s.x))) {
        dirty = true;
    }
    if (m_props.SetValue(SCALE_Y, Variable(s.y))) {
        dirty = true;
    }
    if (m_props.SetValue(SCALE_Z, Variable(s.z))) {
        dirty = true;
    }

    if (dirty) {
        SetDirty(true);
    }
}

void Primitive::SetShear(const sm::vec3& s)
{
    bool dirty = false;

    if (m_props.SetValue(SHEAR_X, Variable(s.x))) {
        dirty = true;
    }
    if (m_props.SetValue(SHEAR_Y, Variable(s.y))) {
        dirty = true;
    }
    if (m_props.SetValue(SHEAR_Z, Variable(s.z))) {
        dirty = true;
    }

    if (dirty) {
        SetDirty(true);
    }
}

void Primitive::InitProps()
{
    m_props.Assign(TRANS_X, PropNames[TRANS_X], Variable(0.0f));
    m_props.Assign(TRANS_Y, PropNames[TRANS_Y], Variable(0.0f));
    m_props.Assign(TRANS_Z, PropNames[TRANS_Z], Variable(0.0f));

    m_props.Assign(ROT_X, PropNames[ROT_X], Variable(0.0f));
    m_props.Assign(ROT_Y, PropNames[ROT_Y], Variable(0.0f));
    m_props.Assign(ROT_Z, PropNames[ROT_Z], Variable(0.0f));

    m_props.Assign(SCALE_X, PropNames[SCALE_X], Variable(1.0f));
    m_props.Assign(SCALE_Y, PropNames[SCALE_Y], Variable(1.0f));
    m_props.Assign(SCALE_Z, PropNames[SCALE_Z], Variable(1.0f));

    m_props.Assign(SHEAR_X, PropNames[SHEAR_X], Variable(0.0f));
    m_props.Assign(SHEAR_Y, PropNames[SHEAR_Y], Variable(0.0f));
    m_props.Assign(SHEAR_Z, PropNames[SHEAR_Z], Variable(0.0f));
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
    assert(NodeHelper::CheckPropsType(*this, 0, MAX_BUILD_IN_PROP, VarType::Float));

    auto& props = m_props.GetProps();

    auto scale = sm::vec3(
        props[SCALE_X].Val().f,
        props[SCALE_Y].Val().f,
        props[SCALE_Z].Val().f
    );

    auto rotate = sm::vec3(
        props[ROT_X].Val().f,
        props[ROT_Y].Val().f,
        props[ROT_Z].Val().f
    ) * SM_DEG_TO_RAD;

    auto translate = sm::vec3(
        props[TRANS_X].Val().f,
        props[TRANS_Y].Val().f,
        props[TRANS_Z].Val().f
    );

    return Transform::CalcTransformMat(translate, rotate, scale, sm::vec3(0, 0, 0));
}

}
}