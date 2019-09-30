#include "sop/node/ForeachPrimEnd.h"
#include "sop/node/ForeachPrimBegin.h"
#include "sop/Evaluator.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/GroupRebuild.h"

#include <queue>

#include <assert.h>

namespace sop
{
namespace node
{

void ForeachPrimEnd::Execute(Evaluator& eval)
{
    auto begin = FindForeachBegin();
    if (!begin) {
        return;
    }

    auto prev_geo = NodeHelper::GetInputGeo(*begin, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Brush);

    auto nodes = FindClosureNodes(begin);

    // prepare eval, closure
    Evaluator sub_eval;
    for (auto& n : nodes) {
        sub_eval.AddNode(n);
    }

    // do
    size_t n_prim = prev_geo->GetAttr().GetPrimtives().size();
    if (m_do_single_pass) {
        if (m_single_pass_offset < static_cast<int>(n_prim)) {
            DoOnePass(sub_eval, begin, prev_geo, m_single_pass_offset);
        }
    } else {
        for (size_t i = 0, n = n_prim; i < n; ++i) {
            DoOnePass(sub_eval, begin, prev_geo, i);
        }
    }

    m_geo_impl->UpdateByAttr();
}

void ForeachPrimEnd::EnableSinglePass(bool do_single_pass)
{
    if (m_do_single_pass == do_single_pass) {
        return;
    }

    m_do_single_pass = do_single_pass;

    SetDirty(true);
}

void ForeachPrimEnd::SetSinglePassOffset(int single_pass_offset)
{
    if (m_single_pass_offset == single_pass_offset) {
        return;
    }

    m_single_pass_offset = single_pass_offset;

    SetDirty(true);
}

NodePtr ForeachPrimEnd::FindForeachBegin() const
{
    std::queue<NodePtr> buf;
    assert(m_imports.size() == 1);
    if (m_imports[0].conns.empty()) {
        return nullptr;
    }

    assert(m_imports[0].conns.size() == 1);
    auto prev_node = m_imports[0].conns[0].node.lock();
    assert(prev_node);
    buf.push(prev_node);
    while (!buf.empty())
    {
        auto n = buf.front(); buf.pop();
        assert(n);

        if (n->get_type() == rttr::type::get<node::ForeachPrimBegin>()) {
            return n;
        }

        for (auto& in : n->GetImports()) {
            for (auto& conn : in.conns) {
                buf.push(conn.node.lock());
            }
        }
    }

    return nullptr;
}

std::set<NodePtr> ForeachPrimEnd::FindClosureNodes(const NodePtr& begin) const
{
    std::set<NodePtr> nodes;

    std::queue<NodePtr> buf;
    buf.push(begin);
    while (!buf.empty())
    {
        auto n = buf.front(); buf.pop();
        assert(n);

        if (n.get() == this) {
            continue;
        }

        nodes.insert(n);
        for (auto& out : n->GetExports()) {
            for (auto& conn : out.conns) {
                buf.push(conn.node.lock());
            }
        }
    }

    return nodes;
}

void ForeachPrimEnd::DoOnePass(Evaluator& sub_eval, const NodePtr& begin,
                               const std::shared_ptr<GeometryImpl>& prev_geo, size_t idx)
{
    {
        auto& prev_prims = prev_geo->GetAttr().GetPrimtives();
        std::vector<bool> del_flags(prev_prims.size(), true);
        del_flags[idx] = false;

        assert(begin->get_type() == rttr::type::get<node::ForeachPrimBegin>());
        auto geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

        GroupRebuild group_rebuild(*geo_impl);

        geo_impl->GetAttr().RemoveItems(GeoAttrType::Primitive, del_flags, true);
        geo_impl->UpdateByAttr();

        std::static_pointer_cast<node::ForeachPrimBegin>(begin)->SetGeoImpl(geo_impl);
    }

    sub_eval.MakeDirty();
    sub_eval.Update();

    auto e_prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (e_prev_geo)
    {
        auto& attr = m_geo_impl->GetAttr();
        m_geo_impl->GetGroup().Combine(e_prev_geo->GetGroup(), attr.GetPoints().size(),
            attr.GetVertices().size(), attr.GetPrimtives().size());
        attr.Combine(e_prev_geo->GetAttr());
    }
}

}
}