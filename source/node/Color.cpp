#include "sop/node/Color.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/GeoAttrDefine.h"
#include "sop/ParmList.h"

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
    const size_t num = attr.GetSize(m_attr_add_to);
    auto list = std::make_shared<ParmFlt3List>(
        GEO_ATTR_CD, std::vector<sm::vec3>(num, m_color)
    );
    attr.AddParmList(m_attr_add_to, list);
}

}
}