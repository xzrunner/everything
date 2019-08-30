#include "everything/node/Delete.h"
#include "everything/GeometryImpl.h"
#include "everything/GeoShape.h"

namespace evt
{
namespace node
{

void Delete::Execute(Evaluator& eval, TreeContext& ctx)
{
    m_geo_impl.reset();

    // only support points now
    if (m_entity_type != EntityType::Points) {
        return;
    }

    auto prev_geo = GetInputGeo(0);
    if (!prev_geo) {
        return;
    }

    std::vector<sm::vec3> vertices;
    for (auto& p : prev_geo->GetAttr().GetPoints()) {
        if (m_exp.Calc(p->pos)) {
            vertices.push_back(p->pos);
        }
    }
    if (!vertices.empty()) {
        m_geo_impl = std::make_shared<GeometryImpl>(GeoPoints(vertices));
    }
}

void Delete::SetDelNonSelected(bool del_non_selected)
{
    if (m_delete_non_selected == del_non_selected) {
        return;
    }

    m_delete_non_selected = del_non_selected;

    SetDirty(true);
}

void Delete::SetEntityType(EntityType type)
{
    if (m_entity_type == type) {
        return;
    }

    m_entity_type = type;

    SetDirty(true);
}

void Delete::SetFilterExp(const std::string& exp)
{
    if (m_filter_exp == exp) {
        return;
    }

    m_filter_exp = exp;
    m_exp.Reset(exp);

    SetDirty(true);
}

}
}