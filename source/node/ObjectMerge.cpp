#include "sop/node/ObjectMerge.h"
#include "sop/GeoAdaptor.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

namespace sop
{
namespace node
{

void ObjectMerge::Execute(Evaluator& eval)
{
    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Brush);
    auto& attr = m_geo_impl->GetAttr();
    for (auto& obj : m_objs)
    {
        auto geo = obj->GetGeometry();
        assert(geo);
        m_geo_impl->GetGroup().Combine(geo->GetGroup(), attr.GetPoints().size(),
            attr.GetVertices().size(), attr.GetPrimtives().size());
        attr.Combine(geo->GetAttr());
    }

    m_geo_impl->UpdateByAttr();
}

void ObjectMerge::SetObjects(const std::vector<NodePtr>& objs)
{
    NODE_PROP_SET(m_objs, objs);
}

}
}