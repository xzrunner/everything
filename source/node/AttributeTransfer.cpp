#include "sop/node/AttributeTransfer.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

namespace sop
{
namespace node
{

void AttributeTransfer::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto to_geo = NodeHelper::GetInputGeo(*this, IDX_TO_GEO);
    if (!to_geo) {
        return;
    }
    auto from_geo = NodeHelper::GetInputGeo(*this, IDX_FROM_GEO);
    if (!from_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*to_geo);

    auto& from_attr = from_geo->GetAttr();
    auto& to_attr = m_geo_impl->GetAttr();
    for (int i = 0, n = static_cast<int>(GeoAttrClass::MaxTypeNum); i < n; ++i)
    {
        const auto cls = static_cast<GeoAttrClass>(i);
        const size_t from_sz = from_attr.GetSize(cls);
        if (from_sz != to_attr.GetSize(cls)) {
            continue;
        }

        auto& from_descs = from_attr.GetAttrDesc(cls);

        for (auto& name : m_attrs[i])
        {
            const int from_idx = from_attr.QueryAttrIdx(cls, name);
            assert(from_idx >= 0);

            std::vector<VarValue> vars;
            vars.reserve(from_sz);
            switch (cls)
            {
            case GeoAttrClass::Point:
                for (auto& p : from_attr.GetPoints()) {
                    vars.push_back(VarValue(p->vars[from_idx]));
                }
                break;

            case GeoAttrClass::Vertex:
                for (auto& v : from_attr.GetVertices()) {
                    vars.push_back(VarValue(v->vars[from_idx]));
                }
                break;

            case GeoAttrClass::Primitive:
                for (auto& prim : from_attr.GetPrimtives()) {
                    vars.push_back(VarValue(prim->vars[from_idx]));
                }
                break;

            case GeoAttrClass::Detail:
                vars.push_back(VarValue(from_attr.GetDetail().vars[from_idx]));
                break;

            default:
                assert(0);
                continue;
            }

            to_attr.AddAttr(cls, from_descs[from_idx], vars);
        }
    }
}

void AttributeTransfer::SetCopyAttrs(GeoAttrClass cls, const std::vector<std::string>& attrs)
{
    const int idx = static_cast<int>(cls);
    if (m_attrs[idx] == attrs) {
        return;
    }

    m_attrs[idx] = attrs;

    SetDirty(true);
}

}
}