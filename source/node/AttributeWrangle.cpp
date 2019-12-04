#include "sop/node/AttributeWrangle.h"
#include "sop/NodeHelper.h"
#include "sop/GeometryImpl.h"
#include "sop/EvalContext.h"
#include "sop/Evaluator.h"

#include <cpputil/StringHelper.h>

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

    auto expr = FormatExprStr();

    EvalContext eval_ctx(eval, *this);
    auto& pts = m_geo_impl->GetAttr().GetPoints();
    eval_ctx.attr_type = GeoAttrClass::Point;
    for (size_t i = 0, n = pts.size(); i < n; ++i)
    {
        eval_ctx.attr_idx = i;
        eval.RunStatement("{" + expr + "}", eval_ctx);
    }

    m_geo_impl->UpdateByAttr();
}

std::string AttributeWrangle::FormatExprStr() const
{
    std::string ret = m_vex_expr;
    cpputil::StringHelper::ReplaceAll(ret, "\\n", "\n");
    cpputil::StringHelper::ReplaceAll(ret, "\\t", "\t");
    return ret;
}

}
}