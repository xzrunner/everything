#include "sop/node/Color.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/GeoAttrDefine.h"

namespace sop
{
namespace node
{

void Color::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    auto& attr = m_geo_impl->GetAttr();

    size_t num = 0;
    switch (m_attr_add_to)
    {
    case GeoAttrClass::Point:
        num = attr.GetPoints().size();
        break;
    case GeoAttrClass::Vertex:
        num = attr.GetVertices().size();
        break;
    case GeoAttrClass::Primitive:
        num = attr.GetPrimtives().size();
        break;
    case GeoAttrClass::Detail:
        num = 1;
        break;
    default:
        assert(0);
    }

    std::vector<VarValue> vars(num, VarValue(m_color));
    attr.AddAttr(m_attr_add_to, GEO_ATTR_CD, vars);
}

}
}