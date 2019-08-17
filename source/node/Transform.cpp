#include "everything/node/Transform.h"
#include "everything/Geometry.h"

namespace evt
{
namespace node
{

void Transform::Execute(TreeContext& ctx)
{
    m_geo.reset();

    auto prev_geo = GetInputGeo(0);
    if (!prev_geo) {
        return;
    }

    m_geo = std::make_shared<Geometry>(*prev_geo);

    auto mat = CalcTransformMat();
    m_geo->TraversePoints([&mat](sm::vec3& p, bool& dirty)->bool
    {
        p = mat * p;
        dirty = true;
        return true;
    });
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