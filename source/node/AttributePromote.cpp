#include "sop/node/AttributePromote.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

namespace sop
{
namespace node
{

void AttributePromote::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    if (m_attr_name.empty() || m_from_type == m_to_type) {
        return;
    }

    auto& attr = m_geo_impl->GetAttr();
    auto attr_idx = attr.QueryAttrIdx(m_from_type, m_attr_name);
    if (attr_idx < 0) {
        return;
    }

    if (m_from_type == GeoAttrClass::Vertex &&
        m_to_type   == GeoAttrClass::Point)
    {
        auto& src_vts = attr.GetVertices();
        auto& dst_pts = attr.GetPoints();

        // add new
        std::vector<sop::VarValue> vars;
        vars.resize(dst_pts.size());
        for (auto& v : src_vts) {
            vars[v->point->attr_idx] = v->vars[attr_idx];
        }
        auto& var_descs = attr.GetAttrDesc(m_from_type);
        attr.AddAttr(m_to_type, var_descs[attr_idx], vars);

        // rm old
        for (auto& v : src_vts) {
            v->vars.erase(v->vars.begin() + attr_idx);
        }
        const_cast<std::vector<GeoAttribute::VarDesc>&>(var_descs)
            .erase(var_descs.begin() + attr_idx);
    }
    else if (m_from_type == GeoAttrClass::Point &&
             m_to_type   == GeoAttrClass::Vertex)
    {
        auto& src_pts = attr.GetPoints();
        auto& dst_vts = attr.GetVertices();

        // add new
        std::vector<sop::VarValue> vars;
        vars.reserve(dst_vts.size());
        for (auto& v : dst_vts) {
            vars.push_back(src_pts[v->point->attr_idx]->vars[attr_idx]);
        }
        auto& var_descs = attr.GetAttrDesc(m_from_type);
        attr.AddAttr(m_to_type, var_descs[attr_idx], vars);

        // rm old
        for (auto& p : src_pts) {
            p->vars.erase(p->vars.begin() + attr_idx);
        }
        const_cast<std::vector<GeoAttribute::VarDesc>&>(var_descs)
            .erase(var_descs.begin() + attr_idx);
    }
}

void AttributePromote::SetAttrName(const std::string& name)
{
    NODE_PROP_SET(m_attr_name, name);
}

void AttributePromote::SetPromoteType(GeoAttrClass from, GeoAttrClass to)
{
    NODE_PROP_SET(m_from_type, from);
    NODE_PROP_SET(m_to_type, to);
}

}
}