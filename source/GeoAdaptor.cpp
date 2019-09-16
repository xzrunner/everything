#include "everything/GeoAdaptor.h"
#include "everything/GroupMgr.h"
#include "everything/GeoAttribute.h"
#include "everything/GeoShape.h"

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

GeoAdaptor::GeoAdaptor(const Type& type)
{
    Init(type);
}

GeoAdaptor::GeoAdaptor(const GeoShape& shape)
{
    Init(shape.Type());
    FromGeoShape(shape);
}

GeoAdaptor::GeoAdaptor(const GeoAdaptor& adaptor)
    : m_type(adaptor.m_type)
    , m_node(adaptor.m_node->Clone())
{
}

void GeoAdaptor::UpdateByBrush(GeoAttribute& attr, const model::BrushModel& brush_model)
{
    assert(m_type == Type::Brush);

    std::shared_ptr<model::Model> model =
        model::BrushBuilder::PolymeshFromBrush(brush_model);
    UpdateModel(model);

    StoreToAttr(attr, brush_model);
}

void GeoAdaptor::StoreBrush(std::unique_ptr<model::BrushModel>& brush_model)
{
    assert(m_type == Type::Brush);

    auto& cmodel_inst = m_node->GetUniqueComp<n3::CompModelInst>();
    std::unique_ptr<model::ModelExtend> model_ext = std::move(brush_model);
    cmodel_inst.GetModel()->SetModelExt(model_ext);
}

void GeoAdaptor::UpdateByAttr(const GeoAttribute& attr)
{
    Type type = Type::Shape;
    for (auto& prim : attr.GetPrimtives()) {
        if (prim->type == GeoAttribute::Primitive::Type::PolygonFace) {
            type = Type::Brush;
        }
    }
    if (type != m_type) {
        Init(type);
    }

    switch (type)
    {
    case GeoShapeType::Points:
    {
        auto& pts = attr.GetPoints();

        std::vector<sm::vec3> vertices;
        vertices.reserve(pts.size());
        for (auto& p : pts) {
            vertices.push_back(p->pos);
        }
        FromGeoShape(GeoPoints(vertices));
    }
        break;
    case GeoShapeType::Polyline:
    {
        auto& vts = attr.GetVertices();

        std::vector<sm::vec3> vertices;
        vertices.reserve(vts.size());
        for (auto& v : vts) {
            vertices.push_back(v->point->pos);
        }
        FromGeoShape(GeoPolyline(vertices));
    }
        break;
    case GeoShapeType::Faces:
    {
        auto brush_model = GetBrushModel();
        assert(brush_model);
        LoadFromAttr(*brush_model, attr);

        assert(m_type == GeoShapeType::Faces);
        std::shared_ptr<model::Model> model =
            model::BrushBuilder::PolymeshFromBrush(*brush_model);
        UpdateModel(model);
    }
        break;
    }
}

std::unique_ptr<GeoShape> GeoAdaptor::ToGeoShape() const
{
    std::unique_ptr<GeoShape> ret = nullptr;
    switch (m_type)
    {
    case GeoShapeType::Points:
    {
        auto shape = GetShape();
        assert(shape->get_type() == rttr::type::get<gs::PointSet3D>());
        auto points = std::static_pointer_cast<gs::PointSet3D>(shape);
        ret = std::make_unique<GeoPoints>(points->GetVertices());
    }
        break;
    case GeoShapeType::Polyline:
    {
        auto shape = GetShape();
        assert(shape->get_type() == rttr::type::get<gs::Polyline3D>());
        auto polyline = std::static_pointer_cast<gs::Polyline3D>(shape);
        if (polyline->GetClosed()) {
            auto vertices = polyline->GetVertices();
            vertices.push_back(vertices.front());
            ret = std::make_unique<GeoPolyline>(vertices);
        } else {
            ret = std::make_unique<GeoPolyline>(polyline->GetVertices());
        }
    }
        break;
    case GeoShapeType::Faces:
        // todo
        break;
    }
    return ret;
}

void GeoAdaptor::FromGeoShape(const GeoShape& shape)
{
    m_type = shape.Type();
    switch (m_type)
    {
    case GeoShapeType::Points:
    {
        auto& vertices = static_cast<const GeoPoints&>(shape).GetVertices();
        auto shape = std::make_shared<gs::PointSet3D>(vertices);

        auto& cshape = m_node->GetSharedComp<n3::CompShape>();
        cshape.SetShape(shape);

        auto& caabb = m_node->GetUniqueComp<n3::CompAABB>();
        caabb.SetAABB(shape->GetBounding());
    }
        break;
    case GeoShapeType::Polyline:
    {
        auto& vertices = static_cast<const GeoPolyline&>(shape).GetVertices();
        if (vertices.size() < 2) {
            return;
        }

        std::shared_ptr<gs::Shape3D> shape = nullptr;
        if (vertices.front() == vertices.back()) {
            auto del_end = vertices;
            del_end.pop_back();
            shape = std::make_shared<gs::Polyline3D>(del_end, true);
        } else {
            shape = std::make_shared<gs::Polyline3D>(vertices, false);
        }

        auto& cshape = m_node->GetSharedComp<n3::CompShape>();
        cshape.SetShape(shape);

        auto& caabb = m_node->GetUniqueComp<n3::CompAABB>();
        caabb.SetAABB(shape->GetBounding());
    }
        break;
    case GeoShapeType::Faces:
        break;
    default:
        assert(0);
    }
}

model::BrushModel* GeoAdaptor::GetBrushModel() const
{
    if (m_type != Type::Brush) {
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

std::shared_ptr<gs::Shape3D> GeoAdaptor::GetShape() const
{
    switch (m_type)
    {
    case evt::GeoShapeType::Points:
    case evt::GeoShapeType::Polyline:
    {
        assert(m_node->HasSharedComp<n3::CompShape>());
        auto& cshape = m_node->GetSharedComp<n3::CompShape>();
        return cshape.GetShape();
    }
        break;
    default:
        return nullptr;
    }
}

void GeoAdaptor::Init(const Type& type)
{
    m_type = type;
    switch (m_type)
    {
    case Type::Shape:
        m_node = ns::NodeFactory::Create3D();
        m_node->AddSharedComp<n3::CompShape>();
        break;
    case Type::Brush:
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

        auto model = std::make_shared<model::Model>();
        UpdateModel(model);

        auto& cmodel_inst = m_node->GetUniqueComp<n3::CompModelInst>();
        std::unique_ptr<model::ModelExtend> model_ext = std::make_unique<model::BrushModel>();
        cmodel_inst.GetModel()->SetModelExt(model_ext);
    }
        break;
    default:
        assert(0);
    }
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
    assert(brushes.size() == 1);
    for (auto& brush : brushes)
    {
        auto& points = brush.impl->Points();
        auto& faces = brush.impl->Faces();

        for (auto& p : points) {
            dst.m_points.push_back(std::make_shared<GeoAttribute::Point>(p));
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

    dst.SetupAABB();
}

void GeoAdaptor::LoadFromAttr(model::BrushModel& dst, const GeoAttribute& src)
{
    std::vector<sm::vec3> points;
    points.reserve(src.m_points.size());
    std::map<std::shared_ptr<GeoAttribute::Point>, size_t> pt2idx;
    size_t idx = 0;
    for (auto& p : src.m_points) {
        points.push_back(p->pos);
        pt2idx.insert({ p, idx++ });
    }

    std::vector<pm3::FacePtr> faces;
    faces.reserve(src.m_primtives.size());
    for (auto& prim : src.m_primtives)
    {
        if (prim->vertices.size() < 3) {
            continue;
        }
        auto face = std::make_shared<pm3::Face>();
        face->points.reserve(prim->vertices.size());
        for (auto& v : prim->vertices)
        {
            auto itr = pt2idx.find(v->point);
            assert(itr != pt2idx.end());
            face->points.push_back(itr->second);
        }
        face->plane = sm::Plane(
            points[face->points[0]],
            points[face->points[1]],
            points[face->points[2]]
        );
        faces.push_back(face);
    }

    model::BrushModel::Brush brush;
    brush.impl = std::make_shared<pm3::Polytope>(points, faces);

    std::vector<model::BrushModel::Brush> brushes;
    brushes.push_back(brush);
    dst.SetBrushes(brushes);
}

}