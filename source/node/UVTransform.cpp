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

    auto mat = Transform::CalcTransformMat(m_translate, m_rotate * SM_DEG_TO_RAD, m_scale, m_shear);
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

void UVTransform::SetGroupName(const std::string& name)
{
    NODE_PROP_SET(m_group_name, name);
}

void UVTransform::SetGroupType(GroupType type)
{
    NODE_PROP_SET(m_group_type, type);
}

}
}