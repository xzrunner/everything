#include "everything/node/Measure.h"
#include "everything/NodeHelper.h"
#include "everything/GeometryImpl.h"

#include <SM_Calc.h>

namespace evt
{
namespace node
{

void Measure::Execute(Evaluator& eval, TreeContext& ctx)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    auto& attr = m_geo_impl->GetAttr();
    switch (m_ms_type)
    {
    case Type::Perimeter:
        attr.AddAttr(GeoAttribute::PRIMITIVE, CalcPerimeter());
        break;
    case Type::Area:
        attr.AddAttr(GeoAttribute::PRIMITIVE, CalcArea());
        break;
    default:
        assert(0);
    }
}

void Measure::SetMesureType(Type type)
{
    if (m_ms_type == type) {
        return;
    }

    m_ms_type = type;

    SetDirty(true);
}

void Measure::SetMesureName(const std::string& name)
{
    if (m_ms_name == name) {
        return;
    }

    m_ms_name = name;

    SetDirty(true);
}

std::shared_ptr<GeoAttribute::AttrList>
Measure::CalcPerimeter() const
{
    auto& attr = m_geo_impl->GetAttr();
    auto& prims = attr.GetPrimtives();
    if (prims.empty()) {
        return nullptr;
    }

    auto attrs = std::make_shared<GeoAttribute::AttrList>();

    attrs->name = m_ms_name.empty() ? "perimeter" : m_ms_name;

    auto shape_type = m_geo_impl->GetShapeType();
    switch (shape_type)
    {
    case GeoShapeType::Points:
        for (auto prim : prims) {
            attrs->vars.push_back(Variable(0.0f));
        }
        break;
    case GeoShapeType::Polyline:
        for (auto prim : prims)
        {
            float len = 0;
            auto& vts = prim->vertices;
            assert(vts.size() > 1);
            for (int i = 0, n = vts.size() - 1; i < n; ++i) {
                len += sm::dis_pos3_to_pos3(
                    vts[i]->point->pos, vts[i + 1]->point->pos
                );
            }
            attrs->vars.push_back(Variable(len));
        }
        break;
    case GeoShapeType::Faces:
        for (auto prim : prims)
        {
            float len = 0;
            auto& vts = prim->vertices;
            assert(vts.size() > 2);
            for (int i = 0, n = vts.size(); i < n; ++i) {
                len += sm::dis_pos3_to_pos3(
                    vts[i]->point->pos, vts[(i + 1) % n]->point->pos
                );
            }
            attrs->vars.push_back(Variable(len));
        }
        break;
    default:
        assert(0);
    }

    return attrs;
}

std::shared_ptr<GeoAttribute::AttrList>
Measure::CalcArea() const
{
    auto& attr = m_geo_impl->GetAttr();
    auto& prims = attr.GetPrimtives();
    if (prims.empty()) {
        return nullptr;
    }

    auto attrs = std::make_shared<GeoAttribute::AttrList>();

    attrs->name = m_ms_name.empty() ? "area" : m_ms_name;

    auto shape_type = m_geo_impl->GetShapeType();
    switch (shape_type)
    {
    case GeoShapeType::Points:
    case GeoShapeType::Polyline:
        for (auto prim : prims) {
            attrs->vars.push_back(Variable(0.0f));
        }
        break;
    case GeoShapeType::Faces:
        // todo
        for (auto prim : prims)
        {
            std::vector<sm::vec3> vts;
            vts.reserve(prim->vertices.size());
            for (auto& v : prim->vertices) {
                vts.push_back(v->point->pos);
            }
            float area = sm::get_polygon_area(vts);
            attrs->vars.push_back(Variable(area));
        }
        break;
    default:
        assert(0);
    }

    return attrs;
}

}
}