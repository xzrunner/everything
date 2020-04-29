#include "sop/node/Box.h"
#include "sop/GeometryImpl.h"

#include <polymesh3/Polytope.h>
#include <model/BrushModel.h>

namespace sop
{
namespace node
{

void Box::Execute(const ur::Device& dev, Evaluator& eval)
{
    m_geo_impl = std::make_shared<GeometryImpl>(dev, GeoAdaptor::Type::Brush);
    BuildModel(dev);
}

void Box::BuildModel(const ur::Device& dev)
{
    if (!m_geo_impl) {
        return;
    }

    auto brush_model = BuildBrush();
    if (brush_model) {
        m_geo_impl->UpdateByBrush(dev, *brush_model);
        m_geo_impl->StoreBrush(brush_model);
    }
}

std::unique_ptr<model::BrushModel>
Box::BuildBrush() const
{
    model::BrushModel::Brush brush;

    brush.desc.mesh_begin = 0;
    brush.desc.mesh_end = 1;
    const int face_num = 6;
    brush.desc.meshes.push_back({ 0, 0, 0, face_num });

    std::vector<pm3::Polytope::FacePtr> faces;
    faces.reserve(face_num);

    if (fabs(m_size.x) < FLT_EPSILON ||
        fabs(m_size.y) < FLT_EPSILON ||
        fabs(m_size.z) < FLT_EPSILON) {
        return nullptr;
    }

    auto h_sz = m_size * 0.5f;
    auto& s = m_scale;
    auto& c = m_pos;
    const float xmin = -h_sz.x * s.x + c.x;
    const float xmax =  h_sz.x * s.x + c.x;
    const float ymin = -h_sz.y * s.y + c.y;
    const float ymax =  h_sz.y * s.y + c.y;
    const float zmin = -h_sz.z * s.z + c.z;
    const float zmax =  h_sz.z * s.z + c.z;

    const sm::vec3 top_left_front (xmin, ymax, zmin);
    const sm::vec3 top_left_back  (xmin, ymax, zmax);
    const sm::vec3 top_right_front(xmax, ymax, zmin);
    const sm::vec3 top_right_back (xmax, ymax, zmax);
    const sm::vec3 btm_left_front (xmin, ymin, zmin);
    const sm::vec3 btm_left_back  (xmin, ymin, zmax);
    const sm::vec3 btm_right_front(xmax, ymin, zmin);
    const sm::vec3 btm_right_back (xmax, ymin, zmax);

    auto top = std::make_shared<pm3::Polytope::Face>();
    top->plane = sm::Plane(top_left_front, top_left_back, top_right_back);
    faces.push_back(top);

    auto bottom = std::make_shared<pm3::Polytope::Face>();
    bottom->plane = sm::Plane(btm_left_front, btm_right_front, btm_right_back);
    faces.push_back(bottom);

    auto left = std::make_shared<pm3::Polytope::Face>();
    left->plane = sm::Plane(top_left_back, top_left_front, btm_left_front);
    faces.push_back(left);

    auto right = std::make_shared<pm3::Polytope::Face>();
    right->plane = sm::Plane(top_right_front, top_right_back, btm_right_back);
    faces.push_back(right);

    auto front = std::make_shared<pm3::Polytope::Face>();
    front->plane = sm::Plane(top_left_front, top_right_front, btm_right_front);
    faces.push_back(front);

    auto back = std::make_shared<pm3::Polytope::Face>();
    back->plane = sm::Plane(top_right_back, top_left_back, btm_left_back);
    faces.push_back(back);

    brush.impl = std::make_shared<pm3::Polytope>(faces);

    auto brush_model = std::make_unique<model::BrushModel>();
    std::vector<model::BrushModel::Brush> brushes;
    brushes.push_back(brush);
    brush_model->SetBrushes(brushes);

    return brush_model;
}

}
}