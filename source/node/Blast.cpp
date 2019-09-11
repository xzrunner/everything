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
        auto& attr = m_geo_impl->GetAttr();
        auto& old_pts = attr.GetPoints();

        std::vector<bool> del_flags;
        SetupDelFlags(*group, old_pts.size(), del_flags);

        std::vector<std::shared_ptr<GeoAttribute::Point>> new_pts;
        assert(del_flags.size() == old_pts.size());
        for (size_t i = 0, n = del_flags.size(); i < n; ++i) {
            if (!del_flags[i]) {
                new_pts.push_back(old_pts[i]);
            }
        }
        if (new_pts.size() != old_pts.size())
        {
            attr.Clear();
            attr.SetPoints(new_pts);
            m_geo_impl->UpdateByAttr();
        }
    }
        break;

    case GroupType::Vertices:
    {
        auto& attr = m_geo_impl->GetAttr();
        auto& old_vts = attr.GetVertices();

        std::vector<bool> del_flags;
        SetupDelFlags(*group, old_vts.size(), del_flags);

        std::vector<std::shared_ptr<GeoAttribute::Vertex>> new_vts;
        assert(del_flags.size() == old_vts.size());
        for (size_t i = 0, n = del_flags.size(); i < n; ++i) {
            if (!del_flags[i]) {
                new_vts.push_back(old_vts[i]);
            }
        }
        if (new_vts.size() != old_vts.size())
        {
            attr.Clear();
            attr.SetVertices(new_vts);
            m_geo_impl->UpdateByAttr();
        }
    }
        break;

    case GroupType::Primitives:
    {
        auto& attr = m_geo_impl->GetAttr();
        auto& old_prims = attr.GetPrimtives();

        std::vector<bool> del_flags;
        SetupDelFlags(*group, old_prims.size(), del_flags);

        std::vector<std::shared_ptr<GeoAttribute::Primitive>> new_prims;
        assert(del_flags.size() == old_prims.size());
        for (size_t i = 0, n = del_flags.size(); i < n; ++i) {
            if (!del_flags[i]) {
                new_prims.push_back(old_prims[i]);
            }
        }
        if (new_prims.size() != old_prims.size()) {
            attr.SetPrimtives(new_prims);
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

void Blast::SetupDelFlags(const Group& group, size_t count,
                          std::vector<bool>& del_flags) const
{
    if (m_delete_non_selected)
    {
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
        }
    }
}

}
}