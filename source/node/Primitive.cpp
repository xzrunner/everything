#include "everything/node/Primitive.h"
#include "everything/node/Transform.h"
#include "everything/NodeHelper.h"
#include "everything/GeometryImpl.h"

namespace evt
{
namespace node
{

void Primitive::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    auto c = m_geo_impl->GetAttr().GetAABB().Center();
    auto mat = Transform::CalcTransformMat(
        m_translate, m_rotate * SM_DEG_TO_RAD, m_scale, m_shear
    );
          
    auto& attr = m_geo_impl->GetAttr();
    for (auto& p : attr.GetPoints()) {
        p->pos -= c;
    }
    for (auto& p : attr.GetPoints()) {
        p->pos = mat * p->pos;
    }
    for (auto& p : attr.GetPoints()) {
        p->pos += c;
    }
    m_geo_impl->UpdateByAttr();
}

void Primitive::SetTranslate(const sm::vec3& t)
{
    if (m_translate == t) {
        return;
    }

    m_translate = t;

    SetDirty(true);
}

void Primitive::SetRotate(const sm::vec3& r)
{
    if (m_rotate == r) {
        return;
    }

    m_rotate = r;

    SetDirty(true);
}

void Primitive::SetScale(const sm::vec3& s)
{
    if (m_scale == s) {
        return;
    }

    m_scale = s;

    SetDirty(true);
}

void Primitive::SetShear(const sm::vec3& s)
{
    if (m_shear == s) {
        return;
    }

    m_shear = s;

    SetDirty(true);
}

}
}