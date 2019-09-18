#include "everything/node/Delete.h"
#include "everything/GeometryImpl.h"
#include "everything/GeoShape.h"
#include "everything/Evaluator.h"
#include "everything/EvalContext.h"
#include "everything/NodeHelper.h"

namespace evt
{
namespace node
{

void Delete::Execute(Evaluator& eval, TreeContext& ctx)
{
    m_geo_impl.reset();

    // only support points now
    if (m_entity_type != EntityType::Points) {
        return;
    }

    auto prev_node = NodeHelper::GetInputNode(*this, 0);
    if (!prev_node) {
        return;
    }
    auto prev_geo = prev_node->GetGeometry();

    std::vector<sm::vec3> vertices;
    EvalContext eval_ctx(eval, *prev_node);
    const auto& src_pts = prev_geo->GetAttr().GetPoints();
    for (int i = 0, n = src_pts.size(); i < n; ++i)
    {
        eval_ctx.point_idx = i;
        auto v = eval.CalcExpr(m_filter_expr, eval_ctx);
        assert(v.type == VarType::Bool);
        if (v.b) {
            vertices.push_back(src_pts[i]->pos);
        }
    }

    if (!vertices.empty()) 
    {
        m_geo_impl = std::make_shared<GeometryImpl>(GeoAdaptor::Type::Shape);

        std::vector<std::shared_ptr<GeoShape>> shapes;
        shapes.reserve(vertices.size());
        for (auto& v : vertices) {
            shapes.push_back(std::make_shared<GeoPoint>(v));
        }
        m_geo_impl->FromGeoShapes(shapes);
    }
}

void Delete::SetDelNonSelected(bool del_non_selected)
{
    if (m_delete_non_selected == del_non_selected) {
        return;
    }

    m_delete_non_selected = del_non_selected;

    SetDirty(true);
}

void Delete::SetEntityType(EntityType type)
{
    if (m_entity_type == type) {
        return;
    }

    m_entity_type = type;

    SetDirty(true);
}

void Delete::SetFilterExpr(const std::string& expr)
{
    if (m_filter_expr == expr) {
        return;
    }

    m_filter_expr = expr;

    SetDirty(true);
}

}
}