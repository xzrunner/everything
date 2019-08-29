#include "everything/node/Box.h"
#include "everything/GeometryImpl.h"

#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>

namespace evt
{
namespace node
{

void Box::Execute(TreeContext& ctx)
{
    m_geo_impl = std::make_shared<GeometryImpl>(GeoShapeType::Faces);
    BuildModel();
}

void Box::SetSize(const sm::vec3& size)
{
    bool dirty = false;

    if (m_props.SetValue(SIZE_X, Variable(size.x))) {
        dirty = true;
    }
    if (m_props.SetValue(SIZE_Y, Variable(size.y))) {
        dirty = true;
    }
    if (m_props.SetValue(SIZE_Z, Variable(size.z))) {
        dirty = true;
    }

    if (dirty) {
        BuildModel();
        SetDirty(true);
    }
}

void Box::SetCenter(const sm::vec3& center)
{
    bool dirty = false;

    if (m_props.SetValue(POS_X, Variable(center.x))) {
        dirty = true;
    }
    if (m_props.SetValue(POS_Y, Variable(center.y))) {
        dirty = true;
    }
    if (m_props.SetValue(POS_Z, Variable(center.z))) {
        dirty = true;
    }

    if (dirty) {
        BuildModel();
        SetDirty(true);
    }
}

void Box::SetScale(const sm::vec3& scale)
{
    if (m_scale == scale) {
        return;
    }

    m_scale = scale;
    BuildModel();
}

void Box::InitProps()
{
    m_props.Assign(SIZE_X, PropNames[SIZE_X], Variable(1.0f));
    m_props.Assign(SIZE_Y, PropNames[SIZE_Y], Variable(1.0f));
    m_props.Assign(SIZE_Z, PropNames[SIZE_Z], Variable(1.0f));
    m_props.Assign(POS_X,  PropNames[POS_X],  Variable(0.0f));
    m_props.Assign(POS_Y,  PropNames[POS_Y],  Variable(0.0f));
    m_props.Assign(POS_Z,  PropNames[POS_Z],  Variable(0.0f));
}

void Box::BuildModel()
{
    if (!m_geo_impl) {
        return;
    }

    auto brush_model = BuildBrush();
    m_geo_impl->UpdateByBrush(*brush_model);
    m_geo_impl->StoreBrush(brush_model);
}

std::unique_ptr<model::BrushModel>
Box::BuildBrush() const
{
    auto& props = m_props.GetProps();
    if (props[SIZE_X].Val().type != VariableType::Float ||
        props[SIZE_Y].Val().type != VariableType::Float ||
        props[SIZE_Z].Val().type != VariableType::Float ||
        props[POS_X].Val().type != VariableType::Float ||
        props[POS_Y].Val().type != VariableType::Float ||
        props[POS_Z].Val().type != VariableType::Float) {
        return nullptr;
    }

    model::BrushModel::Brush brush;

    brush.desc.mesh_begin = 0;
    brush.desc.mesh_end = 1;
    const int face_num = 6;
    brush.desc.meshes.push_back({ 0, 0, 0, face_num });

    std::vector<pm3::FacePtr> faces;
    faces.reserve(face_num);

    const sm::vec3 s = m_scale;
    const sm::vec3 h_sz = sm::vec3(props[SIZE_X].Val().f, props[SIZE_Y].Val().f, props[SIZE_Z].Val().f) * 0.5f;
    const sm::vec3 c = sm::vec3(props[POS_X].Val().f, props[POS_Y].Val().f, props[POS_Z].Val().f);

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