#include "everything/GeoAdaptor.h"
#include "everything/GroupMgr.h"
#include "everything/GeoAttribute.h"

#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>
#include <model/BrushBuilder.h>
#include <geoshape/Point3D.h>
#include <geoshape/PointSet3D.h>
#include <geoshape/Polyline3D.h>
#include <node0/SceneNode.h>
#include <node0/SceneNode.h>
#include <node0/CompMaterial.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <node3/CompShape.h>
#include <node3/CompAABB.h>
#include <ns/NodeFactory.h>
#include <painting3/MaterialMgr.h>

namespace evt
{

GeoAdaptor::GeoAdaptor(DataType type)
    : m_type(type)
{
    switch (type)
    {
    case DataType::Brush:
    {
        m_node = ns::NodeFactory::Create3D();

        m_node->AddSharedComp<n3::CompModel>();
        m_node->AddUniqueComp<n3::CompModelInst>();

        auto& cmaterial = m_node->AddUniqueComp<n0::CompMaterial>();
        auto mat = std::make_unique<pt0::Material>();
        typedef pt3::MaterialMgr::PhongUniforms UNIFORMS;
        mat->AddVar(UNIFORMS::ambient.name, pt0::RenderVariant(sm::vec3(0.04f, 0.04f, 0.04f)));
        mat->AddVar(UNIFORMS::diffuse.name, pt0::RenderVariant(sm::vec3(1, 1, 1)));
        mat->AddVar(UNIFORMS::specular.name, pt0::RenderVariant(sm::vec3(1, 1, 1)));
        mat->AddVar(UNIFORMS::shininess.name, pt0::RenderVariant(50.0f));
        cmaterial.SetMaterial(mat);
    }
        break;
    case DataType::Shape:
    {
        m_node = ns::NodeFactory::Create3D();
        m_node->AddSharedComp<n3::CompShape>();
    }
        break;
    }
}

GeoAdaptor::GeoAdaptor(const GeoAdaptor& adaptor)
    : m_type(adaptor.m_type)
    , m_node(adaptor.m_node->Clone())
{
}

void GeoAdaptor::UpdateByBrush(GeoAttribute& attr, const model::BrushModel& brush_model)
{
    assert(m_type == DataType::Brush);

    std::shared_ptr<model::Model> model =
        model::BrushBuilder::PolymeshFromBrush(brush_model);
    UpdateModel(model);

    StoreToAttr(attr, brush_model);
}

void GeoAdaptor::StoreBrush(std::unique_ptr<model::BrushModel>& brush_model)
{
    auto& cmodel_inst = m_node->GetUniqueComp<n3::CompModelInst>();
    std::unique_ptr<model::ModelExtend> model_ext = std::move(brush_model);
    cmodel_inst.GetModel()->SetModelExt(model_ext);
}

void GeoAdaptor::UpdateByShape(GeoAttribute& attr, const std::shared_ptr<gs::Shape3D>& shape)
{
    auto& cshape = m_node->GetSharedComp<n3::CompShape>();
    cshape.SetShape(shape);

    auto& caabb = m_node->GetUniqueComp<n3::CompAABB>();
    caabb.SetAABB(shape->GetBounding());

    StoreToAttr(attr, *shape);
}

void GeoAdaptor::UpdateByAttr(const GeoAttribute& attr)
{
    auto brush_model = GetBrushModel();
    assert(brush_model);
    LoadFromAttr(*brush_model, attr);

    assert(m_type == DataType::Brush);
    std::shared_ptr<model::Model> model =
        model::BrushBuilder::PolymeshFromBrush(*brush_model);
    UpdateModel(model);
}

model::BrushModel* GeoAdaptor::GetBrushModel() const
{
    if (m_type != DataType::Brush) {
        return nullptr;
    }

    assert(m_node->HasUniqueComp<n3::CompModelInst>());
    auto& cmodel_inst = m_node->GetUniqueComp<n3::CompModelInst>();
    auto& model = cmodel_inst.GetModel();
    assert(model);
    auto& ext = model->GetModelExt();
    assert(ext && ext->Type() == model::EXT_BRUSH);
    auto brush_model = static_cast<model::BrushModel*>(ext.get());
    return brush_model;
}

std::shared_ptr<gs::Shape3D>
GeoAdaptor::GetShape() const
{
    if (m_type != DataType::Shape) {
        return nullptr;
    }

    assert(m_node->HasSharedComp<n3::CompShape>());
    auto& cshape = m_node->GetSharedComp<n3::CompShape>();
    return cshape.GetShape();
}

void GeoAdaptor::UpdateModel(const std::shared_ptr<model::Model>& model)
{
    auto& cmodel = m_node->GetSharedComp<n3::CompModel>();
    cmodel.SetModel(model);

    auto& cmodel_inst = m_node->GetUniqueComp<n3::CompModelInst>();
    cmodel_inst.SetModel(model, 0);

    auto& caabb = m_node->GetUniqueComp<n3::CompAABB>();
    caabb.SetAABB(model->aabb);
}

void GeoAdaptor::StoreToAttr(GeoAttribute& dst, const model::BrushModel& src)
{
    dst.Clear();

    auto& brushes = src.GetBrushes();
    if (brushes.size() != 1) {
        int zz = 0;
    }
    for (auto& brush : brushes)
    {
        auto& points = brush.impl->Points();
        auto& faces = brush.impl->Faces();

        for (auto& p : points)
        {
            auto tmp = std::make_shared<GeoAttribute::Point>();
            tmp->pos = p;
            tmp->point_idx = dst.m_points.size();
            dst.m_points.push_back(tmp);
        }

        for (auto& f : faces)
        {
            auto prim = std::make_shared<GeoAttribute::Primitive>();
            for (auto& p : f->points)
            {
                auto v = std::make_shared<GeoAttribute::Vertex>();
                v->point = dst.m_points[p];
                v->prim  = prim;
                prim->vertices.push_back(v);
                dst.m_vertices.push_back(v);
            }
            dst.m_primtives.push_back(prim);
        }
    }
}

void GeoAdaptor::StoreToAttr(GeoAttribute& dst, const gs::Shape3D& src)
{
    dst.Clear();

    auto type = src.get_type();
    if (type == rttr::type::get<gs::Point3D>())
    {
        auto& point = static_cast<const gs::Point3D&>(src);
        auto tmp = std::make_shared<GeoAttribute::Point>();
        tmp->pos = point.GetPos();
        tmp->point_idx = dst.m_points.size();
        dst.m_points.push_back(tmp);
    }
    else if (type == rttr::type::get<gs::PointSet3D>())
    {
        auto& pos_set = static_cast<const gs::PointSet3D&>(src);
        auto& vertices = pos_set.GetVertices();
        dst.m_points.reserve(vertices.size());
        for (auto& v : vertices) {
            auto tmp = std::make_shared<GeoAttribute::Point>();
            tmp->pos = v;
            tmp->point_idx = dst.m_points.size();
            dst.m_points.push_back(tmp);
        }
    }
    else if (type == rttr::type::get<gs::Polyline3D>())
    {
        auto& polyline = static_cast<const gs::Polyline3D&>(src);
        auto& vertices = polyline.GetVertices();
        dst.m_points.reserve(vertices.size());
        auto prim = std::make_shared<GeoAttribute::Primitive>();
        prim->vertices.reserve(vertices.size());
        for (auto& v : vertices)
        {
            auto dst_p = std::make_shared<GeoAttribute::Point>();
            dst_p->pos = v;
            dst_p->point_idx = dst.m_points.size();
            dst.m_points.push_back(dst_p);

            auto dst_v = std::make_shared<GeoAttribute::Vertex>();
            dst_v->point = dst_p;
            dst_v->prim  = prim;
            dst.m_vertices.push_back(dst_v);

            prim->vertices.push_back(dst_v);
        }
        dst.m_primtives.push_back(prim);
    }
    else
    {
        assert(0);
    }
}

void GeoAdaptor::LoadFromAttr(model::BrushModel& dst, const GeoAttribute& src)
{
    std::vector<sm::vec3> points;
    points.reserve(src.m_points.size());
    for (auto& p : src.m_points) {
        points.push_back(p->pos);
    }

    std::vector<pm3::FacePtr> faces;
    faces.reserve(src.m_primtives.size());
    for (auto& prim : src.m_primtives)
    {
        auto face = std::make_shared<pm3::Face>();
        face->points.reserve(prim->vertices.size());
        for (auto& v : prim->vertices) {
            face->points.push_back(v->point->point_idx);
        }
        faces.push_back(face);
    }

    model::BrushModel::Brush brush;
    brush.impl = std::make_shared<pm3::Polytope>(points, faces);

    std::vector<model::BrushModel::Brush> brushes;
    brushes.push_back(brush);
    dst.SetBrushes(brushes);
}

void GeoAdaptor::LoadFromAttr(gs::Shape3D& dst, const GeoAttribute& src)
{
    auto type = dst.get_type();
    if (type == rttr::type::get<gs::Point3D>())
    {
        assert(src.m_points.size() == 1);

        auto& point = static_cast<gs::Point3D&>(dst);
        point.SetPos(src.m_points[0]->pos);
    }
    else if (type == rttr::type::get<gs::PointSet3D>())
    {
        assert(src.m_vertices.size() == 0);
        assert(src.m_primtives.size() == 0);

        std::vector<sm::vec3> vertices;
        vertices.reserve(src.m_points.size());
        for (auto& p : src.m_points) {
            vertices.push_back(p->pos);
        }

        auto& pos_set = static_cast<gs::PointSet3D&>(dst);
        pos_set.SetVertices(vertices);
    }
    else if (type == rttr::type::get<gs::Polyline3D>())
    {
        assert(src.m_primtives.size() == 1);

        std::vector<sm::vec3> vertices;
        vertices.reserve(src.m_vertices.size());
        for (auto& v : src.m_vertices) {
            vertices.push_back(v->point->pos);
        }

        auto& polyline = static_cast<gs::Polyline3D&>(dst);
        polyline.SetVertices(vertices);
    }
    else
    {
        assert(0);
    }
}

}