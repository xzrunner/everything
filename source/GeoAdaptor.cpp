#include "sop/GeoAdaptor.h"
#include "sop/GroupMgr.h"
#include "sop/GeoAttribute.h"
#include "sop/CompTopoPolyline.h"
#include "sop/ParmList.h"

#include <halfedge/Polyhedron.h>
#include <halfedge/Polyline.h>
#include <polymesh3/Polytope.h>
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

namespace sop
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

    std::shared_ptr<model::Model> model = model::BrushBuilder::PolymeshFromBrushPN(brush_model);
    UpdateModel(model);

    BrushToAttr(attr, brush_model);
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
        std::vector<he::PolylinePtr> polylines;
        AttrToLines(polylines, attr);

        SetTopoLines(polylines);
    }
        break;
    case Type::Brush:
    {
        auto brush_model = GetBrushModel();
        assert(brush_model);
        AttrToBrush(*brush_model, attr);

        auto& prims = attr.GetPrimtives();

        std::vector<std::vector<std::vector<sm::vec2>>> texcoords;
        std::vector<std::vector<std::vector<sm::vec3>>> colors;
        texcoords.resize(brush_model->GetBrushes().size());
        colors.resize(brush_model->GetBrushes().size());

        std::map<size_t, size_t> brush_id2idx;
        for (auto& prim : prims) {
            if (brush_id2idx.find(prim->prim_id) == brush_id2idx.end()) {
                brush_id2idx.insert({ prim->prim_id, 0 });
            }
        }
        size_t idx = 0;
        for (auto& itr : brush_id2idx) {
            itr.second = idx++;
        }

        auto uv_list = attr.QueryParmList(GeoAttrClass::Point, GeoAttr::GEO_ATTR_UV);
        if (uv_list)
        {
            auto& pts = attr.GetPoints();
            assert(uv_list->GetType() == ParmType::Float3);
            auto& uv_data = std::static_pointer_cast<ParmFlt3List>(uv_list)->GetAllItems();
            for (auto& prim : prims)
            {
                std::vector<sm::vec2> uvs(prim->vertices.size());
                for (size_t j = 0, m = prim->vertices.size(); j < m; ++j)
                {
                    auto& v = prim->vertices[j];
                    auto idx = v->point->attr_idx;
                    assert(idx < uv_data.size());
                    auto& uv = uv_data[idx];
                    uvs[j].Set(uv.x, uv.y);
                }

                auto b_idx = brush_id2idx.find(prim->prim_id);
                assert(b_idx != brush_id2idx.end());
                texcoords[b_idx->second].push_back(uvs);
            }
        }
        else
        {
            auto uv_list = attr.QueryParmList(GeoAttrClass::Vertex, GeoAttr::GEO_ATTR_UV);
            if (uv_list)
            {
                auto& vts = attr.GetVertices();
                assert(uv_list->GetType() == ParmType::Float3);
                auto& uv_data = std::static_pointer_cast<ParmFlt3List>(uv_list)->GetAllItems();
                for (auto& prim : prims)
                {
                    std::vector<sm::vec2> uvs(prim->vertices.size());
                    for (size_t j = 0, m = prim->vertices.size(); j < m; ++j)
                    {
                        auto& v = prim->vertices[j];
                        auto idx = v->attr_idx;
                        assert(idx < uv_data.size());
                        auto& uv = uv_data[idx];
                        uvs[j].Set(uv.x, uv.y);
                    }

                    auto b_idx = brush_id2idx.find(prim->prim_id);
                    assert(b_idx != brush_id2idx.end());
                    texcoords[b_idx->second].push_back(uvs);
                }
            }
            else
            {
                texcoords.clear();
            }
        }

        auto cd_list = attr.QueryParmList(GeoAttrClass::Point, GeoAttr::GEO_ATTR_CD);
        if (cd_list)
        {
            auto& pts = attr.GetPoints();
            assert(cd_list->GetType() == ParmType::Vector);
            auto& cd_data = std::static_pointer_cast<ParmFlt3List>(cd_list)->GetAllItems();
            for (auto& prim : prims)
            {
                std::vector<sm::vec3> cds(prim->vertices.size());
                for (size_t j = 0, m = prim->vertices.size(); j < m; ++j)
                {
                    auto& v = prim->vertices[j];
                    auto idx = v->point->attr_idx;
                    assert(idx < cd_data.size());
                    cds[j] = cd_data[idx];
                }

                auto b_idx = brush_id2idx.find(prim->prim_id);
                assert(b_idx != brush_id2idx.end());
                colors[b_idx->second].push_back(cds);
            }
        }
        else
        {
            auto cd_list = attr.QueryParmList(GeoAttrClass::Vertex, GeoAttr::GEO_ATTR_CD);
            if (cd_list)
            {
                auto& vts = attr.GetVertices();
                assert(cd_list->GetType() == ParmType::Vector);
                auto& cd_data = std::static_pointer_cast<ParmFlt3List>(cd_list)->GetAllItems();
                for (auto& prim : prims)
                {
                    std::vector<sm::vec3> cds(prim->vertices.size());
                    for (size_t j = 0, m = prim->vertices.size(); j < m; ++j)
                    {
                        auto& v = prim->vertices[j];
                        auto idx = v->attr_idx;
                        assert(idx < cd_data.size());
                        cds[j] = cd_data[idx];
                    }

                    auto b_idx = brush_id2idx.find(prim->prim_id);
                    assert(b_idx != brush_id2idx.end());
                    colors[b_idx->second].push_back(cds);
                }
            }
            else
            {
                colors.clear();
            }
        }

        std::shared_ptr<model::Model> model = nullptr;
        if (!texcoords.empty()) {
            model = model::BrushBuilder::PolymeshFromBrushPNT(*brush_model, texcoords);
        } else if (!colors.empty()) {
            model = model::BrushBuilder::PolymeshFromBrushPNC(*brush_model, colors);
        } else {
            model = model::BrushBuilder::PolymeshFromBrushPN(*brush_model);
        }

        UpdateModel(model);
    }
        break;
    default:
        assert(0);
    }
}

const std::vector<he::PolylinePtr>&
GeoAdaptor::GetTopoLines() const
{
    assert(m_type == Type::Shape);
    assert(m_node->HasUniqueComp<CompTopoPolyline>());
    auto& cpolyline = m_node->GetUniqueComp<CompTopoPolyline>();
    return cpolyline.GetLines();
}

void GeoAdaptor::SetTopoLines(const std::vector<he::PolylinePtr>& lines)
{
    assert(m_type == Type::Shape);

    std::vector<std::shared_ptr<gs::Shape3D>> gs_shapes;
    for (auto& line : lines)
    {
        auto& pts = line->GetVertices();
        auto& pls = line->GetPolylines();
        if (pls.Size() == 0)
        {
            auto first_point = pts.Head();
            auto curr_point = first_point;
            do {
                gs_shapes.push_back(std::make_shared<gs::Point3D>(curr_point->position));
                curr_point = curr_point->linked_next;
            } while (curr_point != first_point);
        }
        else
        {
            auto first_polyline = pls.Head();
            auto curr_polyline = first_polyline;
            do {
                std::vector<sm::vec3> points;
                auto first_edge = curr_polyline->edge;
                auto curr_edge = first_edge;
                do {
                    points.push_back(curr_edge->vert->position);
                    curr_edge = curr_edge->next;
                } while (curr_edge && curr_edge != first_edge);

                const bool loop = curr_edge == first_edge;
                gs_shapes.push_back(std::make_shared<gs::Polyline3D>(points, loop));

                curr_polyline = curr_polyline->linked_next;
            } while (curr_polyline != first_polyline);
        }
    }

    auto& ctopo = m_node->GetUniqueComp<CompTopoPolyline>();
    ctopo.SetLines(lines);

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

void GeoAdaptor::Init(const Type& type)
{
    m_type = type;
    switch (m_type)
    {
    case Type::Shape:
        m_node = ns::NodeFactory::Create3D();
        m_node->AddSharedComp<n3::CompShape>();
        m_node->AddUniqueComp<CompTopoPolyline>();
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

void GeoAdaptor::BrushToAttr(GeoAttribute& dst, const model::BrushModel& src)
{
    dst.Clear();

    auto& brushes = src.GetBrushes();
    size_t prim_id = 0;
    for (auto& brush : brushes)
    {
        auto& points = brush.impl->Points();
        auto& faces = brush.impl->Faces();

        auto p_off = dst.m_points.size();

        for (auto& p : points)
        {
            auto point = std::make_shared<GeoAttribute::Point>(p->pos);
            point->topo_id = p->topo_id;
            point->prim_id = prim_id;
            dst.m_points.push_back(point);
        }

        for (auto& f : faces)
        {
            auto prim = std::make_shared<GeoAttribute::Primitive>(GeoAttribute::Primitive::Type::PolygonFace);
            prim->topo_id = f->topo_id;
            prim->prim_id = prim_id;
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

        ++prim_id;
    }

    dst.SetupAttrIndices();
    dst.SetupAABB();
}

void GeoAdaptor::AttrToBrush(model::BrushModel& dst, const GeoAttribute& src)
{
    struct Brush
    {
        std::vector<pm3::Polytope::PointPtr> points;
        std::vector<pm3::Polytope::FacePtr>  faces;

        std::map<std::shared_ptr<GeoAttribute::Point>, size_t> pt2idx;
    };
    std::map<int, std::shared_ptr<Brush>> brushes;

    for (auto& p : src.m_points)
    {
        std::shared_ptr<Brush> b = nullptr;
        auto itr = brushes.find(p->prim_id);
        if (itr == brushes.end()) {
            b = std::make_shared<Brush>();
            brushes.insert({ p->prim_id, b });
        } else {
            b = itr->second;
        }

        b->pt2idx.insert({ p, b->points.size() });
        b->points.push_back(std::make_shared<pm3::Polytope::Point>(p->pos, p->topo_id));
    }

    for (auto& prim : src.m_primtives)
    {
        if (prim->vertices.size() < 3) {
            continue;
        }
        auto itr = brushes.find(prim->prim_id);
        if (itr == brushes.end()) {
            continue;
        }

        auto b = itr->second;

        auto face = std::make_shared<pm3::Polytope::Face>();
        face->points.reserve(prim->vertices.size());
        face->topo_id = prim->topo_id;
        for (auto& v : prim->vertices)
        {
            assert(v->point->prim_id == prim->prim_id);
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
    for (auto itr : brushes)
    {
        if (itr.second->faces.empty()) {
            continue;
        }

        model::BrushModel::Brush brush;
        brush.impl = std::make_shared<pm3::Polytope>(
            itr.second->points, itr.second->faces
        );
        model_brushes.push_back(brush);
    }
    dst.SetBrushes(model_brushes);
}

void GeoAdaptor::LinesToAttr(GeoAttribute& dst, const std::vector<he::PolylinePtr>& src)
{
    dst.Clear();

    size_t prim_id = 0;
    for (auto& polyline : src)
    {
        auto& vts = polyline->GetVertices();
        auto& pls = polyline->GetPolylines();

        std::map<he::Vertex*, std::shared_ptr<GeoAttribute::Point>> map_pt;

        auto p_off = dst.m_points.size();

        dst.m_points.reserve(vts.Size());
        auto first_vertex = vts.Head();
        auto curr_vertex = first_vertex;
        do {
            auto point = std::make_shared<GeoAttribute::Point>(curr_vertex->position);
            point->topo_id = curr_vertex->ids;
            point->prim_id = prim_id;
            dst.m_points.push_back(point);
            map_pt.insert({ curr_vertex, point });

            curr_vertex = curr_vertex->linked_next;
        } while (curr_vertex != first_vertex);

        dst.m_primtives.reserve(pls.Size());
        auto first_polyline = pls.Head();
        auto curr_polyline = first_polyline;
        do {
            auto prim = std::make_shared<GeoAttribute::Primitive>(GeoAttribute::Primitive::Type::PolygonFace);
            prim->topo_id = curr_polyline->ids;
            prim->prim_id = prim_id;
            dst.m_primtives.push_back(prim);

            auto first_edge = curr_polyline->edge;
            auto curr_edge = first_edge;
            do {
                auto v = std::make_shared<GeoAttribute::Vertex>();
                auto itr = map_pt.find(curr_edge->vert);
                assert(itr != map_pt.end());
                v->point = itr->second;
                v->prim = prim;
                prim->vertices.push_back(v);
                dst.m_vertices.push_back(v);

                curr_edge = curr_edge->next;
            } while (curr_edge != first_edge);

            curr_polyline = curr_polyline->linked_next;
        } while (curr_polyline != first_polyline);

        ++prim_id;
    }

    dst.SetupAttrIndices();
    dst.SetupAABB();
}

void GeoAdaptor::AttrToLines(std::vector<he::PolylinePtr>& dst, const GeoAttribute& src)
{
    struct Polyline
    {
        std::vector<std::pair<he::TopoID, sm::vec3>> vertices;
        std::vector<std::pair<he::TopoID, std::vector<size_t>>> polylines;

        std::map<std::shared_ptr<GeoAttribute::Point>, size_t> pt2idx;
    };
    std::map<int, std::shared_ptr<Polyline>> polylines;

    for (auto& p : src.m_points)
    {
        std::shared_ptr<Polyline> polyline = nullptr;
        auto itr = polylines.find(p->prim_id);
        if (itr == polylines.end()) {
            polyline = std::make_shared<Polyline>();
            polylines.insert({ p->prim_id, polyline });
        } else {
            polyline = itr->second;
        }

        polyline->pt2idx.insert({ p, polyline->vertices.size() });
        polyline->vertices.push_back({ p->topo_id, p->pos });
    }

    for (auto& prim : src.m_primtives)
    {
        auto itr = polylines.find(prim->prim_id);
        assert(itr != polylines.end());
        if (itr == polylines.end()) {
            continue;
        }

        auto polyline = itr->second;

        std::vector<size_t> dst_indices;
        dst_indices.reserve(prim->vertices.size());
        for (auto& v : prim->vertices)
        {
            assert(v->point->prim_id == prim->prim_id);
            auto itr = polyline->pt2idx.find(v->point);
            assert(itr != polyline->pt2idx.end());
            dst_indices.push_back(itr->second);
        }
        polyline->polylines.push_back({ prim->topo_id, dst_indices });
    }

    dst.reserve(polylines.size());
    for (auto& polyline : polylines) {
        dst.push_back(std::make_shared<he::Polyline>(
            polyline.second->vertices, polyline.second->polylines
        ));
    }
}

}