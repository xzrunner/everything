#include "sop/node/PolyFrame.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

namespace sop
{
namespace node
{

void PolyFrame::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    if (m_entity_type != GroupType::Primitives ||
        m_entity_type != GroupType::Points) {
        return;
    }

    // todo: pts

    switch (m_frame_style)
    {
    case FrameStyle::TwoEdges:
        CreateTwoEdgesAttrs();
        break;
    }
}

void PolyFrame::SetEntityType(GroupType type)
{
    NODE_PROP_SET(m_entity_type, type);
}

void PolyFrame::SetFrameStyle(FrameStyle style)
{
    NODE_PROP_SET(m_frame_style, style);
}

void PolyFrame::SetNormalName(const std::string& name)
{
    NODE_PROP_SET(m_normal_name, name);
}

void PolyFrame::SetTangentName(const std::string& name)
{
    NODE_PROP_SET(m_tangent_name, name);
}

void PolyFrame::SetBitangentName(const std::string& name)
{
    NODE_PROP_SET(m_bitangent_name, name);
}

void PolyFrame::CreateTwoEdgesAttrs()
{

}

}
}