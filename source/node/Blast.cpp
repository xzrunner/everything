#include "everything/node/Blast.h"
#include "everything/GeometryImpl.h"
#include "everything/NodeHelper.h"

namespace evt
{
namespace node
{

void Blast::Execute(Evaluator& eval, TreeContext& ctx)
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
    switch (type)
    {
    case GroupType::Points:
    {
        std::vector<bool> del_flags;
        if (SetupDelFlags(*group, m_geo_impl->GetAttr().GetPoints().size(), del_flags))
        {
            m_geo_impl->GetAttr().RemoveItems(GeoAttrType::Point, del_flags);
            m_geo_impl->UpdateByAttr();
        }
    }
        break;

    case GroupType::Vertices:
    {
        std::vector<bool> del_flags;
        if (SetupDelFlags(*group, m_geo_impl->GetAttr().GetVertices().size(), del_flags))
        {
            m_geo_impl->GetAttr().RemoveItems(GeoAttrType::Vertex, del_flags);
            m_geo_impl->UpdateByAttr();
        }
    }
        break;

    case GroupType::Primitives:
    {
        std::vector<bool> del_flags;
        if (SetupDelFlags(*group, m_geo_impl->GetAttr().GetPrimtives().size(), del_flags))
        {
            m_geo_impl->GetAttr().RemoveItems(GeoAttrType::Primitive, del_flags);
            m_geo_impl->UpdateByAttr();
        }
    }
        break;

    default:
        assert(0);
    }

    if (m_delete_non_selected)
    {
        assert(GetGeoItemNum() == group->items.size());
        for (int i = 0, n = group->items.size(); i < n; ++i) {
            group->items[i] = i;
        }
    }
    else
    {
        group->items.clear();
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

size_t Blast::GetGeoItemNum() const
{
    if (!m_geo_impl) {
        return 0;
    }
    auto group = m_geo_impl->GetGroup().Query(m_group_name);
    if (!group) {
        return 0;
    }

    auto type = m_group_type == GroupType::GuessFromGroup ? group->type : m_group_type;
    switch (type)
    {
    case GroupType::Points:
        return m_geo_impl->GetAttr().GetPoints().size();
    case GroupType::Vertices:
        return m_geo_impl->GetAttr().GetVertices().size();
    case GroupType::Primitives:
        return m_geo_impl->GetAttr().GetPrimtives().size();
    default:
        return 0;
    }
}

}
}