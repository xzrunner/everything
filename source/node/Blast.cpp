#include "everything/node/Blast.h"
#include "everything/NodeHelper.h"
#include "everything/BrushGroup.h"
#include "everything/TreeContext.h"

#include <polymesh3/Brush.h>
#include <model/BrushBuilder.h>
#include <model/BrushModel.h>
#include <node0/SceneNode.h>

namespace evt
{
namespace node
{

void Blast::Execute(TreeContext& ctx)
{
    m_scene_node = NodeHelper::ClonePrevSceneObj(*this, 0);
    if (!m_scene_node) {
        return;
    }

    auto group = ctx.QueryGroup(m_group_name);

    assert(m_scene_node);
    auto old_brush_model = NodeHelper::GetBrushModel(*m_scene_node);
    assert(old_brush_model);
    auto model_ext = old_brush_model->Clone();
    auto brush_model = static_cast<model::BrushModel*>(model_ext.get());
    auto& brushes = brush_model->GetBrushes();

    if (group)
    {
        switch (m_group_type)
        {
        case GroupType::Primitives:
        {
            bool dirty = false;

            assert(brushes.size() == group->parts.size());
            for (int i = 0, n = brushes.size(); i < n; ++i)
            {
                auto& part = group->parts[i];
                if (part.faces.empty()) {
                    continue;
                }

                dirty = true;

                auto& faces = brushes[i].impl->faces;
                std::vector<bool> del_flags;
                if (m_delete_non_selected)
                {
                    del_flags.resize(faces.size(), true);
                    for (auto& f : part.faces) {
                        del_flags[f] = false;
                    }
                }
                else
                {
                    del_flags.resize(faces.size(), false);
                    for (auto& f : part.faces) {
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
            }

            if (dirty) {
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