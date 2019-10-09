#include "sop/node/AttributeCreate.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

namespace sop
{
namespace node
{

void AttributeCreate::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    if (m_attrs.empty()) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    for (auto& attr : m_attrs)
    {
        size_t num = 0;
        switch (attr.cls)
        {
        case GeoAttrType::Point:
            num = m_geo_impl->GetAttr().GetPoints().size();
            break;
        case GeoAttrType::Vertex:
            num = m_geo_impl->GetAttr().GetVertices().size();
            break;
        case GeoAttrType::Primitive:
            num = m_geo_impl->GetAttr().GetPrimtives().size();
            break;
        case GeoAttrType::Detail:
            num = 1;
            break;
        default:
            assert(0);
        }

        std::vector<VarValue> vars(num, attr.value);
        m_geo_impl->GetAttr().AddAttr(attr.cls, GeoAttribute::VarDesc(attr.name, attr.type), vars);
    }
}

void AttributeCreate::AddAttr(const std::string& name, GeoAttrType cls, GeoAttrVarType type, VarValue val)
{
    m_attrs.push_back({ name, cls, type, val });

    SetDirty(true);
}

void AttributeCreate::AddAttr(GeoAttr attr, GeoAttrType cls, VarValue val)
{
    AddAttr(GeoAttrNames[attr], cls, GeoAttrTypes[attr], val);
}

}
}