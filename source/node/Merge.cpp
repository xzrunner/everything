#include "sop/node/Merge.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

namespace sop
{
namespace node
{

void Merge::Execute(Evaluator& eval)
{
    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Brush);

    std::vector<std::shared_ptr<GeometryImpl>> children;
    children.reserve(m_imports.size());
    for (size_t i = 0, n = m_imports.size(); i < n; ++i)
    {
        auto geo = NodeHelper::GetInputGeo(*this, i);
        if (geo) {
            children.push_back(geo);
        }
    }

    auto& attr = m_geo_impl->GetAttr();
    for (auto& c : children)
    {
        m_geo_impl->GetGroup().Combine(c->GetGroup(), attr.GetPoints().size(),
            attr.GetVertices().size(), attr.GetPrimtives().size());
        attr.Combine(c->GetAttr());
    }

    m_geo_impl->UpdateByAttr();
}

void Merge::AddInputPorts(size_t num)
{
    const std::string prefix = "in";
    for (size_t i = 0; i < num; ++i) {
        auto name = prefix + std::to_string(m_imports.size());
        m_imports.push_back({{ NodeVarType::Any, name }});
    }
}

}
}