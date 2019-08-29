#include "everything/node/ForeachPrimBegin.h"
#include "everything/GeometryImpl.h"

namespace evt
{
namespace node
{

ForeachPrimBegin::ForeachPrimBegin()
{
    m_imports = {
        {{ NodeVarType::Any, "in" }}
    };
    m_exports = {
        {{ NodeVarType::Any, "out" }},
    };

    m_geo_impl = std::make_shared<GeometryImpl>(GeoShapeType::Faces);
}

}
}