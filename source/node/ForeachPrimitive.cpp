#include "everything/node/ForeachPrimitive.h"
#include "everything/Geometry.h"
#include "everything/GeoAttrHelper.h"

namespace evt
{
namespace node
{

void ForeachPrimitive::Execute(TreeContext& ctx)
{
    if (!m_begin) {
        return;
    }

    m_geo.reset();

    auto prev_geo = GetInputGeo(0);
    if (!prev_geo) {
        return;
    }

    auto& prev_attr = prev_geo->GetAttr();
    auto& prev_prims = prev_attr.GetPrimtives();
    for (size_t i = 0, n = prev_prims.size(); i < n; ++i)
    {
        auto b_geo = m_begin->GetGeometry();
        auto& b_attr = b_geo->GetAttr();
        GeoAttrHelper::GenAttrFromPrim(b_attr, prev_attr, i);
        b_geo->UpdateByAttr();

        m_eval.MakeDirty();
        m_eval.Update();

        auto e_geo = m_end->GetGeometry();
        auto e_prev_geo = m_end->GetInputGeo(0);
        if (e_prev_geo) {
            e_geo->GetGroup().Combine(e_prev_geo->GetGroup(), e_geo->GetAttr().GetPrimtives().size());
            e_geo->GetAttr().Combine(e_prev_geo->GetAttr());
        }
    }

    m_geo = std::make_shared<Geometry>(*m_end->GetGeometry());
}

void ForeachPrimitive::SetNodes(const std::vector<std::shared_ptr<Node>>& nodes,
                                size_t idx_begin, size_t idx_end)
{
    if (!m_begin) {
        assert(!m_end);
        m_begin = std::make_shared<SubNode>();
        m_end   = std::make_shared<SubNode>();
    }

    assert(idx_begin >= 0 && idx_begin < nodes.size() &&
           idx_end >= 0 && idx_end < nodes.size());
    make_connecting({ m_begin, 0 }, { nodes[idx_begin], 0 });
    make_connecting({ nodes[idx_end], 0 }, { m_end, 0 });

    m_eval.ClearAllNodes();
    m_eval.AddNode(m_begin);
    m_eval.AddNode(m_end);
    for (auto& node : nodes) {
        m_eval.AddNode(node);
    }
}

//////////////////////////////////////////////////////////////////////////
// class ForeachPrimitive::SubNode
//////////////////////////////////////////////////////////////////////////

ForeachPrimitive::SubNode::SubNode()
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