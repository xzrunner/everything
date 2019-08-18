#include "everything/node/Sort.h"
#include "everything/GeometryNode.h"

namespace evt
{
namespace node
{

void Sort::Execute(TreeContext& ctx)
{
    m_geo.reset();

    auto prev_geo = GetInputGeo(0);
    if (!prev_geo) {
        return;
    }

    m_geo = std::make_shared<GeometryNode>(*prev_geo);
}

void Sort::SetKey(Key key)
{
    if (m_key == key) {
        return;
    }

    m_key = key;

    SetDirty(true);
}

}
}
