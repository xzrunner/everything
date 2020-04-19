#include "sop/node/Lattice.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"

namespace sop
{
namespace node
{

void Lattice::Execute(const ur2::Device& dev, Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_node = NodeHelper::GetInputNode(*this, IDX_DATA_SOURCE);
    if (!prev_node) {
        return;
    }
    auto prev_geo = prev_node->GetGeometry();
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
}

}
}