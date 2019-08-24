#include "everything/node/ForeachPrimBegin.h"
#include "everything/Geometry.h"

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

    m_geo = std::make_shared<Geometry>(GeoShapeType::Faces);
}

}
}