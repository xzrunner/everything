#include "everything/node/Transform.h"

#include <node0/SceneNode.h>
#include <node3/CompTransform.h>

namespace evt
{
namespace node
{

void Transform::ExecuteSelf()
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

}
}