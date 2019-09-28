#include "everything/GeoAdaptor.h"
#include "everything/GroupMgr.h"
#include "everything/GeoAttribute.h"
#include "everything/GeoShape.h"

#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>
#include <model/BrushBuilder.h>
#include <geoshape/Point3D.h>
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
    case Type::Shape:
    {
        std::vector<std::shared_ptr<GeoShape>> shapes;

        std::set<std::shared_ptr<GeoAttribute::Point>> points;
        for (auto& p : attr.GetPoints()) {
            points.insert(p);
        }

        for (auto& prim : attr.GetPrimtives())
        {
            assert(prim->type == GeoAttribute::Primitive::Type::PolygonCurves);
            std::vector<sm::vec3> vertices;
            vertices.reserve(prim->vertices.size());
            for (auto& v : prim->vertices) {
                points.erase(v->point);
                vertices.push_back(v->point->pos);
            }
            shapes.push_back(std::make_unique<GeoPolyline>(vertices));
        }

        for (auto& p : points) {
            shapes.push_back(std::make_unique<GeoPoint>(p->pos));
        }

        FromGeoShapes(shapes);
    }
        break;
    case Type::Brush:
    {
        auto brush_model = GetBrushModel();
        assert(brush_model);
        LoadFromAttr(*brush_model, attr);

        std::shared_ptr<model::Model> model =
            model::BrushBuilder::PolymeshFromBrush(*brush_model);
        UpdateModel(model);
    }
        break;
    default:
        assert(0);
    }
}

std::vector<std::shared_ptr<GeoShape>>
GeoAdaptor::ToGeoShapes() const
{
    assert(m_type == Type::Shape);

    std::vector<std::shared_ptr<GeoShape>> ret;

    auto shapes = GetGeoShapes();
    ret.reserve(shapes.size());
    for (auto& s : shapes)
    {
        auto type = s->get_type();
        if (type == rttr::type::get<gs::Point3D>())
        {
            auto point = std::static_pointer_cast<gs::Point3D>(s);
            ret.push_back(std::make_unique<GeoPoint>(point->GetPos()));
        }
        else if (type == rttr::type::get<gs::Polyline3D>())
        {
            auto polyline = std::static_pointer_cast<gs::Polyline3D>(s);
            if (polyline->GetClosed()) {
                auto vertices = polyline->GetVertices();
                vertices.push_back(vertices.front());
                ret.push_back(std::make_unique<GeoPolyline>(vertices));
            } else {
                ret.push_back(std::make_unique<GeoPolyline>(polyline->GetVertices()));
            }
        }
        else
        {
            assert(0);
        }
    }

    return ret;
}

void GeoAdaptor::FromGeoShapes(const std::vector<std::shared_ptr<GeoShape>>& shapes)
{
    if (shapes.empty()) {
        return;
    }

    assert(m_type == Type::Shape);

    std::vector<std::shared_ptr<gs::Shape3D>> gs_shapes;
    for (auto& s : shapes)
    {
        switch (s->Type())
        {
        case GeoShapeType::Point:
        {
            auto src = static_cast<GeoPoint*>(s.get());
            auto dst = std::make_shared<gs::Point3D>(src->GetVertex());
            gs_shapes.push_back(dst);
        }
            break;
        case GeoShapeType::Polyline:
        {
            auto src = static_cast<GeoPolyline*>(s.get());
            std::shared_ptr<gs::Polyline3D> dst = nullptr;
            auto& vts = src->GetVertices();
            if (vts.front() == vts.back()) {
                auto del_end = vts;
                del_end.pop_back();
                dst = std::make_shared<gs::Polyline3D>(del_end, true);
            }
            else {
                dst = std::make_shared<gs::Polyline3D>(vts, false);
            }
            gs_shapes.push_back(dst);
        }
            break;
        default:
            assert(0);
        }
    }

    auto& cshape = m_node->GetSharedComp<n3::CompShape>();
    cshape.SetShapes(gs_shapes);

    pt3::AABB aabb;
    for (auto& s : gs_shapes) {
        aabb.Combine(s->GetBounding(), sm::mat4());
    }
    auto& caabb = m_node->GetUniqueComp<n3::CompAABB>();
    caabb.SetAABB(aabb);
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

std::vector<std::shared_ptr<gs::Shape3D>>
GeoAdaptor::GetGeoShapes() const
{
    if (m_type != Type::Shape) {
        return std::vector<std::shared_ptr<gs::Shape3D>>();
    }

    assert(m_node->HasSharedComp<n3::CompShape>());
    auto& cshape = m_node->GetSharedComp<n3::CompShape>();
    return cshape.GetShapes();
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
    size_t brush_id = 0;
    for (auto& brush : brushes)
    {
        auto& points = brush.impl->Points();
        auto& faces = brush.impl->Faces();

        auto p_off = dst.m_points.size();

        for (auto& p : points)
        {
            auto point = std::make_shared<GeoAttribute::Point>(p->pos);
            point->topo_id  = p->topo_id;
            point->brush_id = brush_id;
            dst.m_points.push_back(point);
        }

        for (auto& f : faces)
        {
            auto prim = std::make_shared<GeoAttribute::Primitive>(GeoAttribute::Primitive::Type::PolygonFace);
            prim->topo_id  = f->topo_id;
            prim->brush_id = brush_id;
            for (auto& p : f->points)
            {
                auto v = std::make_shared<GeoAttribute::Vertex>();
                v->point = dst.m_points[p_off + p];
                v->prim  = prim;
                prim->vertices.push_back(v);
                dst.m_vertices.push_back(v);
            }
            dst.m_primtives.push_back(prim);
        }

        ++brush_id;
    }

    dst.SetupAABB();
}

void GeoAdaptor::LoadFromAttr(model::BrushModel& dst, const GeoAttribute& src)
{
    struct Brush
    {
        std::vector<pm3::PointPtr> points;
        std::vector<pm3::FacePtr>  faces;

        std::map<std::shared_ptr<GeoAttribute::Point>, size_t> pt2idx;
    };
    std::map<int, std::shared_ptr<Brush>> brushes;

    for (auto& p : src.m_points)
    {
        auto point = std::make_shared<pm3::Point>();
        point->pos = p->pos;
        point->topo_id = p->topo_id;

        std::shared_ptr<Brush> b = nullptr;
        auto itr = brushes.find(p->brush_id);
        if (itr == brushes.end()) {
            b = std::make_shared<Brush>();
            brushes.insert({ p->brush_id, b });
        } else {
            b = itr->second;
        }

        b->pt2idx.insert({ p, b->points.size() });
        b->points.push_back(point);
    }

    for (auto& prim : src.m_primtives)
    {
        if (prim->vertices.size() < 3) {
            continue;
        }
        auto itr = brushes.find(prim->brush_id);
        if (itr == brushes.end()) {
            continue;
        }

        auto b = itr->second;

        auto face = std::make_shared<pm3::Face>();
        face->points.reserve(prim->vertices.size());
        face->topo_id = prim->topo_id;
        for (auto& v : prim->vertices)
        {
            assert(v->point->brush_id == prim->brush_id);
            auto itr = b->pt2idx.find(v->point);
            assert(itr != b->pt2idx.end());
            face->points.push_back(itr->second);
        }
        face->plane = sm::Plane(
            b->points[face->points[0]]->pos,
            b->points[face->points[1]]->pos,
            b->points[face->points[2]]->pos
        );
        b->faces.push_back(face);
    }

    std::vector<model::BrushModel::Brush> model_brushes;
    model_brushes.resize(brushes.size());
    size_t i = 0;
    for (auto itr : brushes) {
        model_brushes[i++].impl = std::make_shared<pm3::Polytope>(
            itr.second->points, itr.second->faces
        );
    }
    dst.SetBrushes(model_brushes);
}

}