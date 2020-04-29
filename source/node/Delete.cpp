#include "sop/node/Delete.h"
#include "sop/GeometryImpl.h"
#include "sop/Evaluator.h"
#include "sop/EvalContext.h"
#include "sop/NodeHelper.h"

namespace sop
{
namespace node
{

void Delete::Execute(const ur::Device& dev, Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_node = NodeHelper::GetInputNode(*this, 0);
    if (!prev_node) {
        return;
    }
    auto prev_geo = prev_node->GetGeometry();
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);

    // only support points now
    if (m_entity_type != EntityType::Points) {
        return;
    }

    auto& pts = m_geo_impl->GetAttr().GetPoints();
    std::vector<bool> del_flags(pts.size(), false);

    EvalContext eval_ctx(eval, *prev_node);
    eval_ctx.attr_type = GeoAttrClass::Point;
    for (size_t i = 0, n = pts.size(); i < n; ++i)
    {
        eval_ctx.attr_idx = i;
        auto v = eval.CalcExpr(m_filter_expr, eval_ctx);
        assert(v.type == dag::VarType::Bool);
        if ((v.b && m_del_selected == Delete::NegateSelected::Delete) ||
            (!v.b && m_del_selected == Delete::NegateSelected::Keep)) {
            bool b = false;
            if (v.type == dag::VarType::Float) {
                if (v.f == 0.0f) {
                    b = false;
                } else if (v.f == 1.0f) {
                    b = true;
                } else {
                    assert(0);
                }
            } else if (v.type == dag::VarType::Bool) {
                b = v.b;
            } else {
                assert(0);
            }
            if ((b && m_del_selected == Delete::NegateSelected::Delete) ||
                (!b && m_del_selected == Delete::NegateSelected::Keep)) {
                del_flags[i] = true;
            }
        }
    }

    m_geo_impl->GetAttr().RemoveItems(GeoAttrClass::Point, del_flags, false);
    m_geo_impl->UpdateByAttr(dev);
}

}
}