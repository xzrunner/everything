#include "sop/node/UVTransform.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

namespace sop
{
namespace node
{

void UVTransform::Execute(Evaluator& eval)
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

    auto& attr = m_geo_impl->GetAttr();
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
            auto attr_idx = attr.QueryAttrIdx(GeoAttrClass::Point, GeoAttr::GEO_ATTR_UV);
            if (attr_idx >= 0)
            {
                auto& pts = attr.GetPoints();
                for (auto i : group->GetItems()) {
                    auto uv = static_cast<const sm::vec3*>(pts[i]->vars[attr_idx].p);
                    pts[i]->vars[attr_idx] = VarValue(mat * *uv);
                }
            }
        }
            break;

        case GroupType::Vertices:
        {
            auto attr_idx = attr.QueryAttrIdx(GeoAttrClass::Vertex, GeoAttr::GEO_ATTR_UV);
            if (attr_idx >= 0)
            {
                auto& vts = attr.GetVertices();
                for (auto i : group->GetItems()) {
                    auto uv = static_cast<const sm::vec3*>(vts[i]->vars[attr_idx].p);
                    vts[i]->vars[attr_idx] = VarValue(mat * *uv);
                }
            }
        }
            break;
        }
    }
    else
    {
        auto attr_idx = attr.QueryAttrIdx(GeoAttrClass::Point, GeoAttr::GEO_ATTR_UV);
        if (attr_idx >= 0)
        {
            for (auto& pt : attr.GetPoints()) {
                auto uv = static_cast<const sm::vec3*>(pt->vars[attr_idx].p);
                pt->vars[attr_idx] = VarValue(mat * *uv);
            }
        }
        else
        {
            attr_idx = attr.QueryAttrIdx(GeoAttrClass::Vertex, GeoAttr::GEO_ATTR_UV);
            if (attr_idx >= 0)
            {
                for (auto& vt : attr.GetVertices()) {
                    auto uv = static_cast<const sm::vec3*>(vt->vars[attr_idx].p);
                    vt->vars[attr_idx] = VarValue(mat * *uv);
                }
            }
        }
    }
    m_geo_impl->UpdateByAttr();
}

void UVTransform::SetTranslate(const sm::vec3& t)
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

void UVTransform::SetRotate(const sm::vec3& r)
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

void UVTransform::SetScale(const sm::vec3& s)
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

void UVTransform::SetShear(const sm::vec3& s)
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

void UVTransform::SetGroupName(const std::string& name)
{
    NODE_PROP_SET(m_group_name, name);
}

void UVTransform::SetGroupType(GroupType type)
{
    NODE_PROP_SET(m_group_type, type);
}

sm::mat4 UVTransform::CalcTransformMat(const sm::vec3& translate, const sm::vec3& rotate,
                                     const sm::vec3& scale, const sm::vec3& shear)
{
    auto s_mat = sm::mat4::Scaled(scale.x, scale.y, scale.z);
    auto r_mat = sm::mat4::Rotated(rotate.x, rotate.y, rotate.z);
    auto t_mat = sm::mat4::Translated(translate.x, translate.y, translate.z);
    return t_mat * r_mat * s_mat;
}

void UVTransform::InitProps()
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

sm::mat4 UVTransform::CalcTransformMat() const
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

    return CalcTransformMat(translate, rotate, scale, sm::vec3(0, 0, 0));
}

}
}