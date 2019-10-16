#include "sop/node/Primitive.h"
#include "sop/node/Transform.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

namespace sop
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
    NODE_PROP_SET(m_translate, t);
}

void Primitive::SetRotate(const sm::vec3& r)
{
    NODE_PROP_SET(m_rotate, r);
}

void Primitive::SetScale(const sm::vec3& s)
{
    NODE_PROP_SET(m_scale, s);
}

void Primitive::SetShear(const sm::vec3& s)
{
    NODE_PROP_SET(m_shear, s);
}

}
}