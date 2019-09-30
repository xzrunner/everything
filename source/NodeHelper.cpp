#include "sop/NodeHelper.h"
#include "sop/Node.h"
#include "sop/EvalContext.h"
#include "sop/Evaluator.h"
#include "sop/GeometryImpl.h"

namespace sop
{

bool NodeHelper::IsRoot(const Node& node)
{
    for (auto& in : node.GetImports()) {
        if (!in.conns.empty()) {
            return false;
        }
    }
    return true;
}

NodePtr NodeHelper::GetInputNode(const Node& node, size_t idx)
{
    auto& imports = node.GetImports();
    if (idx < 0 || idx >= imports.size()) {
        return nullptr;
    }

    auto& conns = imports[idx].conns;
    if (conns.empty()) {
        return nullptr;
    }

    assert(imports[idx].conns.size() == 1);
    return imports[idx].conns[0].node.lock();
}

std::shared_ptr<GeometryImpl>
NodeHelper::GetInputGeo(const Node& node, size_t idx)
{
    auto prev_node = GetInputNode(node, idx);
    if (prev_node) {
        return prev_node->GetGeometry();
    } else {
        return nullptr;
    }
}

bool NodeHelper::CheckPropsType(const Node& node, int begin, int end, VarType type)
{
    auto& props = node.GetProps().GetProps();
    for (int i = begin; i < end; ++i) {
        if (props[i].Val().type != type) {
            return false;
        }
    }
    return true;
}

std::vector<size_t>
NodeHelper::SelectGeoByExpr(GroupType type, const Evaluator& eval, const Node& node, const std::string& expr)
{
    std::vector<size_t> ret;
    switch (type)
    {
    case GroupType::Points:
    {
        EvalContext eval_ctx(eval, node);
        auto& pts = node.GetGeometry()->GetAttr().GetPoints();
        for (size_t i = 0, n = pts.size(); i < n; ++i)
        {
            eval_ctx.point_idx = i;
            auto v = eval.CalcExpr(expr, eval_ctx);
            if (v.type == sop::VarType::Invalid) {
                continue;
            }
            assert(v.type == VarType::Bool);
            if (v.b) {
                ret.push_back(i);
            }
        }
    }
        break;

    case GroupType::Vertices:
    {
        EvalContext eval_ctx(eval, node);
        auto& attr = node.GetGeometry()->GetAttr();
        auto& vts = attr.GetVertices();
        for (size_t i = 0, n = vts.size(); i < n; ++i)
        {
            eval_ctx.point_idx = attr.QueryIndex(vts[i]->point);
            auto v = eval.CalcExpr(expr, eval_ctx);
            if (v.type == sop::VarType::Invalid) {
                continue;
            }
            assert(v.type == VarType::Bool);
            if (v.b) {
                ret.push_back(i);
            }
        }
    }
        break;

    case GroupType::Primitives:
    {
        EvalContext eval_ctx(eval, node);
        auto& attr = node.GetGeometry()->GetAttr();
        auto& prims = attr.GetPrimtives();
        for (size_t i = 0, n = prims.size(); i < n; ++i)
        {
            auto& prim = prims[i];
            bool select = true;
            for (auto& v : prim->vertices)
            {
                eval_ctx.point_idx = attr.QueryIndex(v->point);
                auto v = eval.CalcExpr(expr, eval_ctx);
                if (v.type == sop::VarType::Invalid) {
                    select = false;
                    break;
                }
                assert(v.type == VarType::Bool);
                if (!v.b) {
                    select = false;
                    break;
                }
            }
            if (select) {
                ret.push_back(i);
            }
        }
    }
        break;

    default:
        assert(0);
    }
    return ret;
}

}