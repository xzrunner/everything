#include "sop/node/ObjectMerge.h"
#include "sop/GeoAdaptor.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/Evaluator.h"

namespace sop
{
namespace node
{

void ObjectMerge::Execute(const ur::Device& dev, Evaluator& eval)
{
    m_geo_impl = std::make_shared<GeometryImpl>(dev, GeoAdaptor::Type::Brush);
    auto& attr = m_geo_impl->GetAttr();
    for (auto& path : m_obj_paths)
    {
        auto node = eval.QueryNodeByPath(this, path);
        if (!node) {
            continue;
        }
        auto geo = node->GetGeometry();
        assert(geo);
        m_geo_impl->GetGroup().Combine(geo->GetGroup(), attr.GetPoints().size(),
            attr.GetVertices().size(), attr.GetPrimtives().size());
        attr.Combine(geo->GetAttr());
    }

    m_geo_impl->UpdateByAttr(dev);
}

}
}