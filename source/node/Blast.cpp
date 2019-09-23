#include "everything/node/Blast.h"
#include "everything/GeometryImpl.h"
#include "everything/NodeHelper.h"
#include "everything/GroupRebuild.h"

namespace evt
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

    auto& attr = m_geo_impl->GetAttr();

    std::vector<GroupRebuild> groups;
    m_geo_impl->GetGroup().Traverse([&](const Group& group)->bool {
        groups.emplace_back(attr, const_cast<Group&>(group));
        return true;
    });

    auto type = m_group_type == GroupType::GuessFromGroup ? group->type : m_group_type;
    if (type != group->type) {
        return;
    }

    switch (type)
    {
    case GroupType::Points:
    {
        std::vector<bool> del_flags;
        if (SetupDelFlags(*group, attr.GetPoints().size(), del_flags))
        {
            attr.RemoveItems(GeoAttrType::Point, del_flags, false);
            m_geo_impl->UpdateByAttr();
        }
    }
        break;

    case GroupType::Vertices:
    {
        std::vector<bool> del_flags;
        if (SetupDelFlags(*group, attr.GetVertices().size(), del_flags))
        {
            attr.RemoveItems(GeoAttrType::Vertex, del_flags, true);
            m_geo_impl->UpdateByAttr();
        }
    }
        break;

    case GroupType::Primitives:
    {
        std::vector<bool> del_flags;
        if (SetupDelFlags(*group, attr.GetPrimtives().size(), del_flags))
        {
            attr.RemoveItems(GeoAttrType::Primitive, del_flags, true);
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