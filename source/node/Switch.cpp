#include "sop/node/Switch.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

namespace sop
{
namespace node
{

void Switch::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    assert(NodeHelper::CheckPropsType(*this, 0, MAX_BUILD_IN_PROP, VarType::Int));
    auto& props = m_props.GetProps();
    int selected = props[SELECTED].Val().i;
    auto prev_geo = NodeHelper::GetInputGeo(*this, selected);
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
    bool dirty = false;

    if (m_props.SetValue(SELECTED, Variable(static_cast<int>(select)))) {
        dirty = true;
    }

    if (dirty) {
        SetDirty(true);
    }
}

void Switch::InitProps()
{
    m_props.Assign(SELECTED, PropNames[SELECTED], Variable(0));
}

}
}