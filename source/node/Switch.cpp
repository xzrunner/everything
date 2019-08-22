#include "everything/node/Switch.h"
#include "everything/Geometry.h"

namespace evt
{
namespace node
{

void Switch::Execute(TreeContext& ctx)
{
    m_geo.reset();

    auto prev_geo = GetInputGeo(m_selected);
    if (!prev_geo) {
        return;
    }

    m_geo = std::make_shared<Geometry>(*prev_geo);
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