#include "everything/node/Blast.h"
#include "everything/Geometry.h"

namespace evt
{
namespace node
{

void Blast::Execute(TreeContext& ctx)
{
    m_geo.reset();

    auto prev_geo = GetInputGeo(0);
    if (!prev_geo) {
        return;
    }

    m_geo = std::make_shared<Geometry>(*prev_geo);

    auto group = m_geo->QueryGroup(m_group_name);
    if (!group) {
        return;
    }

    switch (m_group_type)
    {
    case GroupType::Primitives:
    {
        auto& attr = m_geo->GetAttr();
        auto& old_prims = attr.GetPrimtives();

        std::vector<bool> del_flags;
        if (m_delete_non_selected)
        {
            del_flags.resize(old_prims.size(), true);
            for (auto& f : group->items) {
                del_flags[f] = false;
            }
        }
        else
        {
            del_flags.resize(old_prims.size(), false);
            for (auto& f : group->items) {
                del_flags[f] = true;
            }
        }

        std::vector<std::shared_ptr<GeoAttribute::Primitive>> new_prims;
        assert(del_flags.size() == old_prims.size());
        for (size_t i = 0, n = del_flags.size(); i < n; ++i) {
            if (!del_flags[i]) {
                new_prims.push_back(old_prims[i]);
            }
        }
        if (new_prims.size() != old_prims.size()) {
            attr.SetPrimtives(new_prims);
            m_geo->UpdateByAttr();
        }

        break;
    }
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

}
}