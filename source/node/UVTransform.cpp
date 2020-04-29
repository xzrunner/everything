#include "sop/node/UVTransform.h"
#include "sop/node/Transform.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/ParmList.h"

namespace sop
{
namespace node
{

void UVTransform::Execute(const ur::Device& dev, Evaluator& eval)
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
            auto list = attr.QueryParmList(GeoAttrClass::Point, GeoAttr::GEO_ATTR_UV);
            if (list)
            {
                assert(list->GetType() == ParmType::Float3);
                auto& data = std::static_pointer_cast<ParmFlt3List>(list)->GetAllItems();
                for (auto i : group->GetItems()) {
                    const_cast<sm::vec3&>(data[i]) = mat * data[i];
                }
            }
        }
            break;

        case GroupType::Vertices:
        {
            auto list = attr.QueryParmList(GeoAttrClass::Vertex, GeoAttr::GEO_ATTR_UV);
            if (list)
            {
                assert(list->GetType() == ParmType::Float3);
                auto& data = std::static_pointer_cast<ParmFlt3List>(list)->GetAllItems();
                for (auto i : group->GetItems()) {
                    const_cast<sm::vec3&>(data[i]) = mat * data[i];
                }
            }
        }
            break;
        }
    }
    else
    {
        auto list = attr.QueryParmList(GeoAttrClass::Point, GeoAttr::GEO_ATTR_UV);
        if (list)
        {
            assert(list->GetType() == ParmType::Float3);
            for (auto& v : std::static_pointer_cast<ParmFlt3List>(list)->GetAllItems()) {
                const_cast<sm::vec3&>(v) = mat * v;
            }
        }

        list = attr.QueryParmList(GeoAttrClass::Vertex, GeoAttr::GEO_ATTR_UV);
        if (list)
        {
            assert(list->GetType() == ParmType::Float3);
            for (auto& v : std::static_pointer_cast<ParmFlt3List>(list)->GetAllItems()) {
                const_cast<sm::vec3&>(v) = mat * v;
            }
        }
    }
    m_geo_impl->UpdateByAttr(dev);
}

}
}