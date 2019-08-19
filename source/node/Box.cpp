#include "everything/node/Box.h"
#include "everything/Geometry.h"

#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>

namespace evt
{
namespace node
{

void Box::Execute(TreeContext& ctx)
{
    m_geo = std::make_shared<Geometry>(GeoAdaptor::DataType::Brush);
    BuildModel();
}

void Box::SetSize(const sm::vec3& size)
{
    if (m_size == size) {
        return;
    }

    m_size = size;
    BuildModel();

    SetDirty(true);
}

void Box::SetCenter(const sm::vec3& center)
{
    if (m_center == center) {
        return;
    }

    m_center = center;
    BuildModel();
}

void Box::SetScale(const sm::vec3& scale)
{
    if (m_scale == scale) {
        return;
    }

    m_scale = scale;
    BuildModel();
}

void Box::BuildModel()
{
    if (m_geo) 
    {
        auto brush_model = BuildBrush();
        m_geo->UpdateByBrush(*brush_model);
        m_geo->StoreBrush(brush_model);
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

    std::vector<pm3::FacePtr> faces;
    faces.reserve(face_num);

    const sm::vec3 s = m_scale;
    const sm::vec3 h_sz = m_size * 0.5f;
    const sm::vec3 c = m_center;

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

    auto top = std::make_shared<pm3::Face>();
    top->plane = sm::Plane(top_left_front, top_left_back, top_right_back);
    faces.push_back(top);

    auto bottom = std::make_shared<pm3::Face>();
    bottom->plane = sm::Plane(btm_left_front, btm_right_front, btm_right_back);
    faces.push_back(bottom);

    auto left = std::make_shared<pm3::Face>();
    left->plane = sm::Plane(top_left_back, top_left_front, btm_left_front);
    faces.push_back(left);

    auto right = std::make_shared<pm3::Face>();
    right->plane = sm::Plane(top_right_front, top_right_back, btm_right_back);
    faces.push_back(right);

    auto front = std::make_shared<pm3::Face>();
    front->plane = sm::Plane(top_left_front, top_right_front, btm_right_front);
    faces.push_back(front);

    auto back = std::make_shared<pm3::Face>();
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