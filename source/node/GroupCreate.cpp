#include "sop/node/GroupCreate.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"
#include "sop/EvalContext.h"
#include "sop/Evaluator.h"

#include <SM_Calc.h>
#include <model/BrushModel.h>
#include <polymesh3/Polytope.h>
#include <halfedge/Polyhedron.h>

namespace sop
{
namespace node
{

void GroupCreate::Execute(const ur2::Device& dev, Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, IDX_SOURCE_OBJ);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    auto& group_mgr = m_geo_impl->GetGroup();

    auto group = std::make_shared<Group>();
    group->SetName(m_group_name);
    group->SetType(m_group_type);

    if (m_base_group_enable)
    {
        if (m_base_group_syntax.empty()) {
            SelectAll(*group);
        } else {
            group->SetItems(NodeHelper::SelectGeoByExpr(m_group_type, eval, *this, m_base_group_syntax));
        }
    }
    else if (m_keep_in_bounding)
    {
        auto b_geo = NodeHelper::GetInputGeo(*this, IDX_BOUNDING_OBJ);
        if (b_geo) {
            SelectByBoundings(*group, b_geo);
        }
    }
    else if (m_keep_by_normals)
    {
        SelectByNormals(*group);
    }

    group_mgr.Add(group, m_group_merge);
    SetGroupName(group->GetName());
}

void GroupCreate::SelectByNormals(Group& group)
{
    switch (m_group_type)
    {
    case GroupType::Points:
        break;
    case GroupType::Vertices:
        break;
    case GroupType::Edges:
        break;
    case GroupType::Primitives:
    {
        auto& prims = m_geo_impl->GetAttr().GetPrimtives();
        for (size_t i = 0, n = prims.size(); i < n; ++i)
        {
            auto normal = prims[i]->CalcNormal();
            auto angle = sm::get_angle(sm::vec3(0, 0, 0), m_keep_by_normals_dir, normal);
            if (angle <= m_keep_by_normals_angle * SM_DEG_TO_RAD) {
                group.Add(i);
            }
        }
    }
        break;
    default:
        assert(0);
    }
}

void GroupCreate::SelectByBoundings(Group& group, std::shared_ptr<GeometryImpl>& bounding)
{
    auto brush_model = bounding->GetBrushModel();
    if (!brush_model) {
        return;
    }

    auto& brushes = brush_model->GetBrushes();
    switch (m_group_type)
    {
    case GroupType::Points:
    {
        auto& pts = m_geo_impl->GetAttr().GetPoints();
        for (size_t i = 0, n = pts.size(); i < n; ++i) {
            for (auto& b : brushes) {
                if (b.impl->GetTopoPoly()->IsContain(pts[i]->pos)) {
                    group.Add(i);
                    break;
                }
            }
        }
    }
        break;
    case GroupType::Vertices:
        break;
    case GroupType::Edges:
        break;
    case GroupType::Primitives:
        break;
    default:
        assert(0);
    }
}

void GroupCreate::SelectAll(Group& group)
{
    switch (m_group_type)
    {
    case GroupType::Points:
    {
        auto& pts = m_geo_impl->GetAttr().GetPoints();
        std::vector<size_t> items(pts.size());
        for (size_t i = 0, n = pts.size(); i < n; ++i) {
            items[i] = i;
        }
        group.SetItems(items);
    }
        break;
    case GroupType::Vertices:
    {
        auto& vertices = m_geo_impl->GetAttr().GetVertices();
        std::vector<size_t> items(vertices.size());
        for (size_t i = 0, n = vertices.size(); i < n; ++i) {
            items[i] = i;
        }
        group.SetItems(items);
    }
        break;
    case GroupType::Edges:
        break;
    case GroupType::Primitives:
    {
        auto& prims = m_geo_impl->GetAttr().GetPrimtives();
        std::vector<size_t> items(prims.size());
        for (size_t i = 0, n = prims.size(); i < n; ++i) {
            items[i] = i;
        }
        group.SetItems(items);
    }
        break;
    default:
        assert(0);
    }
}

}
}