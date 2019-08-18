#include "everything/node/Carve.h"
#include "everything/GeometryNode.h"

#include <SM_Calc.h>
#include <geoshape/Polyline3D.h>
#include <node0/SceneNode.h>
#include <node3/CompShape.h>
#include <node3/CompAABB.h>

namespace evt
{
namespace node
{

void Carve::Execute(TreeContext& ctx)
{
    m_geo.reset();

    auto prev_geo = GetInputGeo(0);
    if (!prev_geo) {
        return;
    }

    // only support polyline3d now
    auto shape = prev_geo->GetShape();
    if (shape->get_type() != rttr::type::get<gs::Polyline3D>()) {
        return;
    }

    auto polyline = std::static_pointer_cast<gs::Polyline3D>(shape);
    auto& vertices = polyline->GetVertices();
    if (vertices.size() < 2) {
        return;
    }

    std::vector<float> each_len;
    each_len.reserve(vertices.size());
    float tot_len = 0;
    for (int i = 0, n = vertices.size() - 1; i < n; ++i) {
        tot_len += sm::dis_pos3_to_pos3(vertices[i], vertices[i+1]);
        each_len.push_back(tot_len);
    }

    // only support first_u and second_u
    float len_min, len_max;
    if (m_first_u < m_second_u) {
        len_min = tot_len * m_first_u;
        len_max = tot_len * m_second_u;
    } else {
        len_min = tot_len * m_second_u;
        len_max = tot_len * m_first_u;
    }

    auto calc_pos = [&](float len, sm::vec3& pos)->size_t
    {
        for (int i = 0, n = vertices.size() - 1; i < n; ++i)
        {
            auto& curr_len = each_len[i];
            if (len >= curr_len) {
                continue;
            }

            float prev_len = 0.0f;
            if (i > 0) {
                prev_len = each_len[i - 1];
            }
            assert(len >= prev_len && len < curr_len);
            const float p = (len - prev_len) / (curr_len - prev_len);
            pos = vertices[i] + (vertices[i + 1] - vertices[i]) * p;
            return i;
        }
        assert(fabs(len - each_len.back()) < std::numeric_limits<float>::epsilon());
        pos = vertices.back();
        return vertices.size() - 1;
    };

    sm::vec3 start, end;
    size_t s_idx = calc_pos(len_min, start);
    size_t e_idx = calc_pos(len_max, end);
    assert(s_idx <= e_idx);

    std::vector<sm::vec3> dst_vertices;
    dst_vertices.reserve(e_idx - s_idx + 1);
    dst_vertices.push_back(start);
    for (size_t i = s_idx + 1; i <= e_idx; ++i) {
        dst_vertices.push_back(vertices[i]);
    }
    if (end != vertices[e_idx]) {
        dst_vertices.push_back(end);
    }

    m_geo = std::make_shared<GeometryNode>(GeometryNode::DataType::Shape);
    BuildModel(dst_vertices);
}

void Carve::SetFirstU(float u)
{
    u = std::min(1.0f, std::max(0.0f, u));
    if (u == m_first_u) {
        return;
    }

    m_first_u = u;

    SetDirty(true);
}

void Carve::SetSecondU(float u)
{
    u = std::min(1.0f, std::max(0.0f, u));
    if (u == m_second_u) {
        return;
    }

    m_second_u = u;

    SetDirty(true);
}

void Carve::SetFirstV(float v)
{
    v = std::min(1.0f, std::max(0.0f, v));
    if (v == m_first_v) {
        return;
    }

    m_first_v = v;

    SetDirty(true);
}

void Carve::SetSecondV(float v)
{
    v = std::min(1.0f, std::max(0.0f, v));
    if (v == m_second_v) {
        return;
    }

    m_second_v = v;

    SetDirty(true);
}

void Carve::BuildModel(const std::vector<sm::vec3>& vertices)
{
    if (!m_geo) {
        return;
    }
    if (vertices.empty()) {
        return;
    }

    auto shape = std::make_shared<gs::Polyline3D>(vertices);

    auto node = m_geo->GetNode();
    assert(node);

    auto& cshape = node->GetSharedComp<n3::CompShape>();
    cshape.SetShape(shape);

    auto& caabb = node->GetUniqueComp<n3::CompAABB>();
    caabb.SetAABB(shape->GetBounding());
}

}
}