#include "everything/node/Blast.h"
#include "everything/NodeHelper.h"
#include "everything/TreeContext.h"

#include <model/BrushBuilder.h>
#include <model/BrushModel.h>
#include <node0/SceneNode.h>

namespace evt
{
namespace node
{

void Blast::Execute(TreeContext& ctx)
{
    auto obj = NodeHelper::GetInputSceneNode(*this, 0);
    if (!obj) {
        return;
    }
    m_scene_node = obj->Clone();
    auto old_brush_model = NodeHelper::GetBrushModel(*m_scene_node);
    assert(old_brush_model);
    auto model_ext = old_brush_model->Clone();
    auto brush_model = static_cast<model::BrushModel*>(model_ext.get());
    auto& brushes = brush_model->GetBrushes();
    assert(brushes.size() == 1);
    auto& brush = brushes[0];

    auto group = brush.impl->QueryGroup(m_group_name);
    if (group)
    {
        switch (m_group_type)
        {
        case pm3::GroupType::Face:
        {
            auto faces = brush.impl->Faces();
            std::vector<bool> del_flags;
            if (m_delete_non_selected)
            {
                del_flags.resize(faces.size(), true);
                for (auto& f : group->items) {
                    del_flags[f] = false;
                }
            }
            else
            {
                del_flags.resize(faces.size(), false);
                for (auto& f : group->items) {
                    del_flags[f] = true;
                }
            }
            int idx = 0;
            for (auto itr = faces.begin(); itr != faces.end(); ++idx)
            {
                if (del_flags[idx]) {
                    itr = faces.erase(itr);
                } else {
                    ++itr;
                }
            }

            if (faces.size() != brush.impl->Faces().size())
            {
                group->items.clear();
                if (m_delete_non_selected) {
                    for (int i = 0, n = faces.size(); i < n; ++i) {
                        group->items.push_back(i);
                    }
                }

                brush.impl->SetFaces(faces);

                NodeHelper::BuildPolymesh(*m_scene_node, *brush_model);
                NodeHelper::StoreBrush(*m_scene_node, model_ext);
            }
            break;
        }
        }
    }
    else
    {

    }
}

void Blast::SetGroupType(pm3::GroupType type)
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