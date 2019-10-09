#include "sop/node/Blast.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/GeoAttrRebuild.h"
#include "sop/GroupRebuild.h"

namespace sop
{
namespace node
{

void Blast::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    auto group = m_geo_impl->GetGroup().Query(m_group_name);
    if (!group) {
        return;
    }

    auto type = m_group_type == GroupType::GuessFromGroup ? group->type : m_group_type;
    if (type != group->type) {
        return;
    }

    GroupRebuild group_rebuild(*m_geo_impl);
    GeoAttrRebuild attr_rebuild(*m_geo_impl);

    auto& attr = m_geo_impl->GetAttr();
    switch (type)
    {
    case GroupType::Points:
    {
        std::vector<bool> del_flags;
        if (SetupDelFlags(*group, attr.GetPoints().size(), del_flags))
        {
            attr.RemoveItems(GeoAttrClass::Point, del_flags, false);
            m_geo_impl->UpdateByAttr();
        }
    }
        break;

    case GroupType::Vertices:
    {
        std::vector<bool> del_flags;
        if (SetupDelFlags(*group, attr.GetVertices().size(), del_flags))
        {
            attr.RemoveItems(GeoAttrClass::Vertex, del_flags, true);
            m_geo_impl->UpdateByAttr();
        }
    }
        break;

    case GroupType::Primitives:
    {
        std::vector<bool> del_flags;
        if (SetupDelFlags(*group, attr.GetPrimtives().size(), del_flags))
        {
            attr.RemoveItems(GeoAttrClass::Primitive, del_flags, true);
            m_geo_impl->UpdateByAttr();
        }
    }
        break;

    default:
        assert(0);
    }
}

void Blast::SetGroupType(GroupType type)
{
    if (m_group_type == type) {
        return;
    }

    m_group_type = type;

    SetDirty(true);
}

void Blast::SetDeleteNonSelected(bool del_non_selected)
{
    if (m_delete_non_selected == del_non_selected) {
        return;
    }

    m_delete_non_selected = del_non_selected;

    SetDirty(true);
}

bool Blast::SetupDelFlags(const Group& group, size_t count,
                          std::vector<bool>& del_flags) const
{
    bool dirty = false;
    if (m_delete_non_selected)
    {
        if (count > 0) {
            dirty = true;
        }
        del_flags.resize(count, true);
        for (auto& f : group.items) {
            del_flags[f] = false;
        }
    }
    else
    {
        del_flags.resize(count, false);
        for (auto& f : group.items) {
            del_flags[f] = true;
            dirty = true;
        }
    }
    return dirty;
}

}
}