#include "sop/node/Delete.h"
#include "sop/GeometryImpl.h"
#include "sop/Evaluator.h"
#include "sop/EvalContext.h"
#include "sop/NodeHelper.h"

namespace sop
{
namespace node
{

void Delete::Execute(Evaluator& eval)
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
        assert(v.type == VarType::Bool);
        if ((v.b && !m_delete_non_selected) ||
            (!v.b && m_delete_non_selected)) {
            bool b = false;
            if (v.type == VarType::Float) {
                if (v.f == 0.0f) {
                    b = false;
                } else if (v.f == 1.0f) {
                    b = true;
                } else {
                    assert(0);
                }
            } else if (v.type == VarType::Bool) {
                b = v.b;
            } else {
                assert(0);
            }
            if ((b && !m_delete_non_selected) ||
                (!b && m_delete_non_selected)) {
                del_flags[i] = true;
            }
        }
    }

    m_geo_impl->GetAttr().RemoveItems(GeoAttrClass::Point, del_flags, false);
    m_geo_impl->UpdateByAttr();
}

void Delete::SetDelNonSelected(bool del_non_selected)
{
    NODE_PROP_SET(m_delete_non_selected, del_non_selected);
}

void Delete::SetEntityType(EntityType type)
{
    NODE_PROP_SET(m_entity_type, type);
}

void Delete::SetFilterExpr(const std::string& expr)
{
    NODE_PROP_SET(m_filter_expr, expr);
}

}
}