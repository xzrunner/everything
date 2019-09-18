#include "everything/node/ForeachPrimEnd.h"
#include "everything/node/ForeachPrimBegin.h"
#include "everything/Evaluator.h"
#include "everything/GeometryImpl.h"
#include "everything/NodeHelper.h"

#include <queue>

#include <assert.h>

namespace evt
{
namespace node
{

void ForeachPrimEnd::Execute(Evaluator& eval, TreeContext& ctx)
{
    std::set<NodePtr> nodes;
    auto begin = FindForeachBegin(nodes);
    if (!begin) {
        return;
    }

    auto prev_geo = NodeHelper::GetInputGeo(*begin, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Brush);

    // closure
    Evaluator sub_eval;
    for (auto& n : nodes) {
        sub_eval.AddNode(n);
    }

    // foreach prim
    auto& prev_prims = prev_geo->GetAttr().GetPrimtives();
    auto& attr = m_geo_impl->GetAttr();
    std::vector<bool> del_flags(prev_prims.size(), true);
    for (size_t i = 0, n = prev_prims.size(); i < n; ++i)
    {
        assert(begin->get_type() == rttr::type::get<node::ForeachPrimBegin>());
        auto geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
        del_flags[i] = false;
        if (i != 0) {
            del_flags[i - 1] = true;
        }
        geo_impl->GetAttr().RemoveItems(GeoAttrType::Primitive, del_flags);
        geo_impl->UpdateByAttr();

        std::static_pointer_cast<node::ForeachPrimBegin>(begin)->SetGeoImpl(geo_impl);

        sub_eval.MakeDirty();
        sub_eval.Update();

        auto e_prev_geo = NodeHelper::GetInputGeo(*this, 0);
        if (e_prev_geo)
        {
            m_geo_impl->GetGroup().Combine(e_prev_geo->GetGroup(), attr.GetPoints().size(),
                attr.GetVertices().size(), attr.GetPrimtives().size());
            attr.Combine(e_prev_geo->GetAttr());
        }
    }

    m_geo_impl->UpdateByAttr();
}

NodePtr ForeachPrimEnd::FindForeachBegin(std::set<NodePtr>& nodes) const
{
    NodePtr begin = nullptr;

    std::queue<NodePtr> buf;
    assert(m_imports.size() == 1);
    if (m_imports[0].conns.empty()) {
        return begin;
    }
    assert(m_imports[0].conns.size() == 1);
    auto prev_node = m_imports[0].conns[0].node.lock();
    assert(prev_node);
    buf.push(prev_node);
    while (!buf.empty())
    {
        auto n = buf.front(); buf.pop();
        assert(n);
        nodes.insert(n);

        if (n->get_type() == rttr::type::get<node::ForeachPrimBegin>()) {
            begin = n;
            continue;
        }

        for (auto& in : n->GetImports()) {
            for (auto& conn : in.conns) {
                auto prev = conn.node.lock();
                buf.push(prev);
            }
        }
    }

    return begin;
}

}
}