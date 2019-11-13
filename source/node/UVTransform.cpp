#include "sop/node/UVTransform.h"
#include "sop/node/Transform.h"
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
    if (m_props.SetValue(TRANS, Variable(t))) {
        SetDirty(true);
    }
}

void UVTransform::SetRotate(const sm::vec3& r)
{
    if (m_props.SetValue(ROT, Variable(r))) {
        SetDirty(true);
    }
}

void UVTransform::SetScale(const sm::vec3& s)
{
    if (m_props.SetValue(SCALE, Variable(s))) {
        SetDirty(true);
    }
}

void UVTransform::SetShear(const sm::vec3& s)
{
    if (m_props.SetValue(SHEAR, Variable(s))) {
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

void UVTransform::InitProps()
{
    m_props.Assign(TRANS, PropNames[TRANS], Variable(sm::vec3(0.0f, 0.0f, 0.0f)));
    m_props.Assign(ROT,   PropNames[ROT],   Variable(sm::vec3(0.0f, 0.0f, 0.0f)));
    m_props.Assign(SCALE, PropNames[SCALE], Variable(sm::vec3(1.0f, 1.0f, 1.0f)));
    m_props.Assign(SHEAR, PropNames[SHEAR], Variable(sm::vec3(0.0f, 0.0f, 0.0f)));
}

sm::mat4 UVTransform::CalcTransformMat() const
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