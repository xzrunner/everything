#include "everything/node/Switch.h"
#include "everything/GeometryImpl.h"

namespace evt
{
namespace node
{

void Switch::Execute(TreeContext& ctx)
{
    m_geo_impl.reset();

    auto prev_geo = GetInputGeo(m_selected);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
}

void Switch::AddInputPorts(size_t num)
{
    const std::string prefix = "in";
    for (size_t i = 0; i < num; ++i) {
        auto name = prefix + std::to_string(m_imports.size());
        m_imports.push_back({ { NodeVarType::Any, name } });
    }
}

void Switch::SetSelected(size_t select)
{
    if (m_selected == select) {
        return;
    }

    m_selected = select;

    SetDirty(true);
}

}
}