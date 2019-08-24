#include "everything/node/Merge.h"
#include "everything/Geometry.h"

namespace evt
{
namespace node
{

void Merge::Execute(TreeContext& ctx)
{
    m_geo = std::make_shared<Geometry>(GeoShapeType::Faces);

    std::vector<std::shared_ptr<Geometry>> children;
    for (auto& port : m_imports)
    {
        if (port.conns.empty()) {
            continue;
        }

        assert(port.conns.size() == 1);
        auto src_obj = port.conns[0].node.lock();
        if (!src_obj) {
            continue;
        }

        children.push_back(src_obj->GetGeometry());
    }

    auto& attr = m_geo->GetAttr();
    for (auto& c : children) {
        attr.Combine(c->GetAttr());
        m_geo->GetGroup().Combine(c->GetGroup(), attr.GetPrimtives().size());
    }
    attr.ResetPointsOrder();

    m_geo->UpdateByAttr();
}

void Merge::AddInputPorts(size_t num)
{
    const std::string prefix = "in";
    for (size_t i = 0; i < num; ++i) {
        auto name = prefix + std::to_string(m_imports.size());
        m_imports.push_back({{ NodeVarType::Any, name }});
    }
}

}
}