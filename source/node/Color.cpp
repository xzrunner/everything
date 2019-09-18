#include "everything/node/Color.h"
#include "everything/NodeHelper.h"
#include "everything/GeometryImpl.h"
#include "everything/GeoAttrName.h"

namespace evt
{
namespace node
{

void Color::Execute(Evaluator& eval, TreeContext& ctx)
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
    case GeoAttrType::Point:
        num = attr.GetPoints().size();
        break;
    case GeoAttrType::Vertex:
        num = attr.GetVertices().size();
        break;
    case GeoAttrType::Primitive:
        num = attr.GetPrimtives().size();
        break;
    case GeoAttrType::Detail:
        num = 1;
        break;
    default:
        assert(0);
    }

    std::vector<VarValue> r_vars(num, VarValue(m_color.x));
    std::vector<VarValue> g_vars(num, VarValue(m_color.y));
    std::vector<VarValue> b_vars(num, VarValue(m_color.z));
    attr.AddAttr(m_attr_add_to, { GeoAttrName::col_x, VarType::Float }, r_vars);
    attr.AddAttr(m_attr_add_to, { GeoAttrName::col_y, VarType::Float }, g_vars);
    attr.AddAttr(m_attr_add_to, { GeoAttrName::col_z, VarType::Float }, b_vars);
}

void Color::SetAttrAddTo(GeoAttrType attr)
{
    if (m_attr_add_to == attr) {
        return;
    }

    m_attr_add_to = attr;

    SetDirty(true);
}

void Color::SetColor(const sm::vec3& col)
{
    if (m_color == col) {
        return;
    }

    m_color = col;

    SetDirty(true);
}

}
}