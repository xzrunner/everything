#include "sop/node/Measure.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/ParmList.h"

#include <SM_Calc.h>

namespace sop
{
namespace node
{

void Measure::Execute(const ur2::Device& dev, Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    auto& attr = m_geo_impl->GetAttr();
    switch (m_type)
    {
    case Type::Perimeter:
    {
        std::string name;
        std::vector<float> val;
        CalcPerimeter(name, val);

        attr.AddParmList(GeoAttrClass::Primitive,
            std::make_shared<ParmFltList>(name, ParmType::Float, val)
        );
    }
        break;
    case Type::Area:
    {
        std::string name;
        std::vector<float> val;
        CalcArea(name, val);

        attr.AddParmList(GeoAttrClass::Primitive,
            std::make_shared<ParmFltList>(name, ParmType::Float, val)
        );
    }
        break;
    default:
        assert(0);
    }
}

void Measure::CalcPerimeter(std::string& name, std::vector<float>& val) const
{
    name = m_attr_name.empty() ? "perimeter" : m_attr_name;

    auto& attr = m_geo_impl->GetAttr();
    auto& prims = attr.GetPrimtives();
    if (prims.empty()) {
        return;
    }

    auto type = m_geo_impl->GetAdaptorType();
    switch (type)
    {
    case GeoAdaptor::Type::Shape:
        for (auto prim : prims)
        {
            assert(prim->type == GeoAttribute::Primitive::Type::PolygonCurves);
            float len = 0;
            auto& vts = prim->vertices;
            assert(vts.size() > 1);
            for (int i = 0, n = vts.size() - 1; i < n; ++i) {
                len += sm::dis_pos3_to_pos3(
                    vts[i]->point->pos, vts[i + 1]->point->pos
                );
            }
            val.push_back(len);
        }
        break;
    case GeoAdaptor::Type::Brush:
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
            val.push_back(len);
        }
        break;
    default:
        assert(0);
    }
}

void Measure::CalcArea(std::string& name, std::vector<float>& val) const
{
    name = m_attr_name.empty() ? "area" : m_attr_name;

    auto& attr = m_geo_impl->GetAttr();
    auto& prims = attr.GetPrimtives();
    if (prims.empty()) {
        return;
    }

    auto type = m_geo_impl->GetAdaptorType();
    switch (type)
    {
    case GeoAdaptor::Type::Shape:
        for (auto prim : prims) {
            val.push_back(0.0f);
        }
        break;
    case GeoAdaptor::Type::Brush:
        // todo
        for (auto prim : prims)
        {
            std::vector<sm::vec3> vts;
            vts.reserve(prim->vertices.size());
            for (auto& v : prim->vertices) {
                vts.push_back(v->point->pos);
            }
            float area = sm::get_polygon_area(vts);
            val.push_back(area);
        }
        break;
    default:
        assert(0);
    }
}

}
}