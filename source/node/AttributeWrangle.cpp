#include "sop/node/AttributeWrangle.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/EvalContext.h"
#include "sop/Evaluator.h"

namespace sop
{
namespace node
{

void AttributeWrangle::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
 
    EvalContext eval_ctx(eval, *this);
    auto& pts = m_geo_impl->GetAttr().GetPoints();
    eval_ctx.attr_type = GeoAttrClass::Point;
    for (size_t i = 0, n = pts.size(); i < n; ++i)
    {
        eval_ctx.attr_idx = i;
        eval.RunStatement("{" + m_vex_expr + "}", eval_ctx);
    }
}

void AttributeWrangle::SetVexExpr(const std::string& expr)
{
    NODE_PROP_SET(m_vex_expr, expr);
}

}
}