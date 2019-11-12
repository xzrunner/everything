#include "sop/node/Python.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

namespace sop
{
namespace node
{

void Python::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (prev_geo) {
        m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    }
}

void Python::SetCode(const std::string& code)
{
    NODE_PROP_SET(m_code, code);
}

}
}