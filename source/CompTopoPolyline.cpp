#include "sop/CompTopoPolyline.h"

namespace sop
{

const char* const CompTopoPolyline::TYPE_NAME = "sop_topo_polyline";

CompTopoPolyline::CompTopoPolyline()
{
}

std::unique_ptr<n0::NodeComp> CompTopoPolyline::Clone(const n0::SceneNode& node) const
{
    auto comp = std::make_unique<CompTopoPolyline>();
    comp->m_lines.reserve(m_lines.size());
    for (auto& src : m_lines) {
        auto dst = std::make_shared<he::Polyline>(*src);
        comp->m_lines.push_back(dst);
    }
    return comp;
}

}