#include "everything/node/Transform.h"

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

        auto& pt = prev_node->GetUniqueComp<n3::CompTransform>();
        auto& t = m_scene_node->GetUniqueComp<n3::CompTransform>();

        t.SetPosition(pt.GetPosition() + m_translate);

        t.SetAngle(pt.GetAngle().Rotated(sm::Quaternion::CreateFromEulerAngle(m_rotate.z, m_rotate.x, m_rotate.y)));

        auto s = pt.GetScale();
        s.x *= m_scale.x;
        s.y *= m_scale.y;
        s.z *= m_scale.z;
        t.SetScale(s);
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

}
}