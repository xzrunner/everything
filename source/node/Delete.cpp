#include "sop/node/Delete.h"
#include "sop/GeometryImpl.h"
#include "sop/Evaluator.h"
#include "sop/EvalContext.h"
#include "sop/NodeHelper.h"

#include <halfedge/Polyline.h>

namespace sop
{
namespace node
{

void Delete::Execute(Evaluator& eval)
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
    if (!prev_geo) {
        return;
    }

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

        std::vector<std::pair<he::TopoID, sm::vec3>> line_vertices;
        std::vector<std::pair<he::TopoID, std::vector<size_t>>> line_polylines;
        vertices.reserve(vertices.size());
        for (auto& v : vertices) {
            line_vertices.push_back({ he::TopoID(), v });
        }
        auto line = std::make_shared<he::Polyline>(line_vertices, line_polylines);
        m_geo_impl->SetTopoLines({ line });
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