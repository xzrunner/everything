#include "sop/NodeHelper.h"
#include "sop/Node.h"
#include "sop/EvalContext.h"
#include "sop/Evaluator.h"
#include "sop/GeometryImpl.h"
#include "sop/node/Geometry.h"

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
    auto in_node = imports[idx].conns[0].node.lock();
    if (in_node->get_type().is_derived_from<node::Geometry>())
    {
        const int output_idx = imports[idx].conns[0].idx;
        return std::static_pointer_cast<node::Geometry>(in_node)->QueryOutput(output_idx);
    }
    else
    {
        return in_node;
    }
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

std::vector<size_t>
NodeHelper::SelectGeoByExpr(GroupType type, const Evaluator& eval, const Node& node, const std::string& expr)
{
    std::vector<size_t> ret;
    switch (type)
    {
    case GroupType::Points:
    {
        EvalContext eval_ctx(eval, node);
        eval_ctx.attr_type = GeoAttrClass::Point;

        auto& pts = node.GetGeometry()->GetAttr().GetPoints();
        for (size_t i = 0, n = pts.size(); i < n; ++i)
        {
            eval_ctx.attr_idx = i;
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
        eval_ctx.attr_type = GeoAttrClass::Point;

        auto& attr = node.GetGeometry()->GetAttr();
        auto& vts = attr.GetVertices();
        for (size_t i = 0, n = vts.size(); i < n; ++i)
        {
            eval_ctx.attr_idx = vts[i]->point->attr_idx;
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
            // test prim
            {
                eval_ctx.attr_type = GeoAttrClass::Primitive;
                eval_ctx.attr_idx  = i;
                auto v = eval.CalcExpr(expr, eval_ctx);
                if (v.type != sop::VarType::Invalid)
                {
                    assert(v.type == VarType::Bool);
                    if (v.b) {
                        ret.push_back(i);
                        continue;
                    }
                }
            }
            // test each points
            {
                bool select = true;
                for (auto& v : prim->vertices)
                {
                    eval_ctx.attr_type = GeoAttrClass::Point;
                    eval_ctx.attr_idx = v->point->attr_idx;
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
    }
        break;

    default:
        assert(0);
    }
    return ret;
}

}