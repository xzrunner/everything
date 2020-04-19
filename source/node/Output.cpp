#include "sop/node/Output.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

namespace sop
{
namespace node
{

void Output::Execute(const ur2::Device& dev, Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (prev_geo) {
        m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    }
}

}
}