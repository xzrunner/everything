#include "everything/node/Transform.h"
#include "everything/NodeHelper.h"

#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>
#include <node0/SceneNode.h>
#include <node3/CompTransform.h>

namespace evt
{
namespace node
{

void Transform::Execute(TreeContext& ctx)
{
    assert(m_imports.size() == 1);
    if (m_imports[0].conns.empty()) {
        return;
    }

    assert(m_imports[0].conns.size() == 1);

    auto prev = m_imports[0].conns[0].node.lock();
    if (!prev) {
        return;
    }

    auto prev_node = prev->GetSceneNode();
    if (!prev_node)
    {
        m_scene_node.reset();
    }
    else
    {
        if (!m_scene_node) {
            m_scene_node = prev_node->Clone();
        }

        auto old_brush_model = NodeHelper::GetBrushModel(*m_scene_node);
        assert(old_brush_model);
        auto model_ext = old_brush_model->Clone();
        auto brush_model = static_cast<model::BrushModel*>(model_ext.get());
        auto& brushes = brush_model->GetBrushes();
        assert(brushes.size() == 1);
        auto& brush = brushes[0];

        auto mat = CalcTransformMat();
        for (auto& v : brush.impl->Points()) {
            v = mat * v;
        }
        NodeHelper::BuildPolymesh(*m_scene_node, *brush_model);
        NodeHelper::StoreBrush(*m_scene_node, model_ext);
    }
}

void Transform::SetTranslate(const sm::vec3& t)
{
    if (m_translate == t) {
        return;
    }

    m_translate = t;

    SetDirty(true);
}

void Transform::SetRotate(const sm::vec3& r)
{
    if (m_rotate == r) {
        return;
    }

    m_rotate = r;

    SetDirty(true);
}

void Transform::SetScale(const sm::vec3& s)
{
    if (m_scale == s) {
        return;
    }

    m_scale = s;

    SetDirty(true);
}

void Transform::SetShear(const sm::vec3& s)
{
    if (m_shear == s) {
        return;
    }

    m_shear = s;

    SetDirty(true);
}

sm::mat4 Transform::CalcTransformMat() const
{
    auto s_mat = sm::mat4::Scaled(m_scale.x, m_scale.y, m_scale.z);

    auto rot = m_rotate * SM_DEG_TO_RAD;
    auto r_mat = sm::mat4::Rotated(rot.x, rot.y, rot.z);

    auto t_mat = sm::mat4::Translated(m_translate.x, m_translate.y, m_translate.z);

    return s_mat * r_mat * t_mat;
}

}
}