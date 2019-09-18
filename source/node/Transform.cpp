#include "everything/node/Transform.h"
#include "everything/GeometryImpl.h"
#include "everything/NodeHelper.h"

namespace evt
{
namespace node
{

void Transform::Execute(Evaluator& eval, TreeContext& ctx)
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
        auto type = m_group_type == GroupType::GuessFromGroup ? group->type : m_group_type;
        switch (type)
        {
        case GroupType::Points:
        {
            auto& pts = m_geo_impl->GetAttr().GetPoints();
            for (auto i : group->items) {
                pts[i]->pos = mat * pts[i]->pos;
            }
        }
            break;

        case GroupType::Vertices:
        {
            std::set<std::shared_ptr<GeoAttribute::Point>> pts;
            auto& vts = m_geo_impl->GetAttr().GetVertices();
            for (auto i : group->items) {
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
            for (auto i : group->items) {
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

void Transform::SetRotate(const sm::vec3& r)
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

void Transform::SetScale(const sm::vec3& s)
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

void Transform::SetShear(const sm::vec3& s)
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

void Transform::SetGroupType(GroupType type)
{
    if (m_group_type == type) {
        return;
    }

    m_group_type = type;

    SetDirty(true);
}

void Transform::InitProps()
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

sm::mat4 Transform::CalcTransformMat() const
{
    assert(NodeHelper::CheckPropsType(*this, 0, MAX_BUILD_IN_PROP, VarType::Float));

    auto& props = m_props.GetProps();

    auto s_mat = sm::mat4::Scaled(
        props[SCALE_X].Val().f,
        props[SCALE_Y].Val().f,
        props[SCALE_Z].Val().f
    );

    auto rot = sm::vec3(props[ROT_X].Val().f, props[ROT_Y].Val().f, props[ROT_Z].Val().f) * SM_DEG_TO_RAD;
    auto r_mat = sm::mat4::Rotated(rot.x, rot.y, rot.z);

    auto t_mat = sm::mat4::Translated(
        props[TRANS_X].Val().f,
        props[TRANS_Y].Val().f,
        props[TRANS_Z].Val().f
    );

    return s_mat * r_mat * t_mat;
}

}
}