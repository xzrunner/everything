#include "everything/node/Delete.h"
#include "everything/Geometry.h"

#include <geoshape/PointSet3D.h>

#include <assert.h>

namespace evt
{
namespace node
{

void Delete::Execute(TreeContext& ctx)
{
    m_geo.reset();

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
    if (vertices.empty()) {
        return;
    }

    m_geo = std::make_shared<Geometry>(GeoAdaptor::DataType::Shape);
    BuildModel(vertices);
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

void Delete::BuildModel(const std::vector<sm::vec3>& vertices)
{
    if (m_geo && !vertices.empty()) {
        auto shape = std::make_shared<gs::PointSet3D>(vertices);
        m_geo->UpdateByShape(shape);
    }
}

}
}