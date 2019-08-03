#include "everything/node/Blast.h"
#include "everything/NodeHelper.h"
#include "everything/BrushGroup.h"
#include "everything/TreeContext.h"

#include <polymesh3/Brush.h>
#include <model/BrushBuilder.h>
#include <model/BrushModel.h>
#include <node0/SceneNode.h>
#include <node3/CompModel.h>

namespace evt
{
namespace node
{

void Blast::BeforeUpdateContext()
{
    m_group = nullptr;
}

void Blast::SetGroupType(GroupType type)
{
    if (m_group_type == type) {
        return;
    }

    m_group_type = type;

    Execute(false);
}

void Blast::SetDeleteNonSelected(bool del_non_selected)
{
    if (m_delete_non_selected == del_non_selected) {
        return;
    }

    m_delete_non_selected = del_non_selected;

    Execute(false);
}

void Blast::ExecuteSelf()
{
    m_scene_node = NodeHelper::ClonePrevSceneObj(*this, 0);
    if (!m_scene_node) {
        return;
    }

    assert(m_scene_node && m_scene_node->HasSharedComp<n3::CompModel>());
    auto brush_model = NodeHelper::GetBrushModel(m_scene_node);
    assert(brush_model);
    auto& brushes = brush_model->GetBrushes();

    if (m_group)
    {
        switch (m_group_type)
        {
        case GroupType::Primitives:
        {
            bool dirty = false;

            assert(brushes.size() == m_group->parts.size());
            for (int i = 0, n = brushes.size(); i < n; ++i)
            {
                auto& part = m_group->parts[i];
                if (part.faces.empty()) {
                    continue;
                }

                dirty = true;

                auto& faces = brushes[i].impl->faces;
                for (auto& f : part.faces) {
                    faces[f] = nullptr;
                }
                for (auto itr = faces.begin(); itr != faces.end(); )
                {
                    if (!*itr) {
                        itr = faces.erase(itr);
                    }
                    else {
                        ++itr;
                    }
                }
            }

            if (dirty)
            {
                NodeHelper::UpdateModelFromBrush(*m_scene_node, *brush_model);

                assert(m_scene_node && m_scene_node->HasSharedComp<n3::CompModel>());
                auto& src_cmodel = m_scene_node->GetSharedComp<n3::CompModel>();
                auto model = src_cmodel.GetModel();

                model::BrushBuilder::UpdateVBO(*model, *brush_model);
            }
            break;
        }
        }
    }
    else
    {

    }
}

void Blast::UpdateCtxSelf(TreeContext& ctx)
{
    auto group = ctx.QueryGroup(m_group_name);
    if (group) {
        m_group = group;
    }
}

}
}