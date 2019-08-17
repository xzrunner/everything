#include "everything/Geometry.h"

#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
#include <model/ModelExtend.h>
#include <model/BrushModel.h>
#include <model/BrushBuilder.h>
#include <painting3/MaterialMgr.h>
#include <geoshape/Shape3D.h>
#include <geoshape/Point3D.h>
#include <geoshape/PointSet3D.h>
#include <geoshape/Polyline3D.h>
#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node0/CompMaterial.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <node3/CompShape.h>
#include <node3/CompAABB.h>

#include <assert.h>

namespace evt
{

Geometry::Geometry(DataType type)
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

Geometry::Geometry(const Geometry& geo)
    : m_type(geo.m_type)
    , m_node(geo.m_node->Clone())
{
    CopyGroups(geo);
}

Geometry& Geometry::operator = (const Geometry& geo)
{
    m_type = geo.m_type;
    m_node = geo.m_node->Clone();
    CopyGroups(geo);

    return *this;
}

void Geometry::TraversePoints(std::function<bool(sm::vec3&, bool& dirty)> func,
                              const std::string& group_name) const
{
    if (!m_node) {
        return;
    }

    if (m_node->HasSharedComp<n3::CompModel>())
    {
        bool dirty = false;

        auto brush_model = GetBrushModel();
        assert(brush_model);
        auto& brushes = brush_model->GetBrushes();
        bool stop = false;
        for (auto& brush : brushes)
        {
            for (auto& v : brush.impl->Points())
            {
                bool d;
                if (!func(v, d)) {
                    stop = true;
                }
                if (d) {
                    dirty = true;
                }

                if (stop) {
                    break;
                }
            }
            if (stop) {
                break;
            }
        }

        if (dirty) {
            UpdateModel(*brush_model);
        }
    }
    else if (m_node->HasSharedComp<n3::CompShape>())
    {
        bool dirty;

        auto shape = GetShape();
        auto type = shape->get_type();
        if (type == rttr::type::get<gs::Point3D>())
        {
            auto point = std::static_pointer_cast<gs::Point3D>(shape);
            func(const_cast<sm::vec3&>(point->GetPos()), dirty);
        }
        else if (type == rttr::type::get<gs::PointSet3D>())
        {
            auto pos_set = std::static_pointer_cast<gs::PointSet3D>(shape);
            for (auto& v : pos_set->GetVertices()) {
                if (!func(const_cast<sm::vec3&>(v), dirty)) {
                    break;
                }
            }
        }
        else if (type == rttr::type::get<gs::Polyline3D>())
        {
            auto polyline = std::static_pointer_cast<gs::Polyline3D>(shape);
            for (auto& v : polyline->GetVertices()) {
                if (!func(const_cast<sm::vec3&>(v), dirty)) {
                    break;
                }
            }
        }
        else
        {
            assert(0);
        }
    }
}

void Geometry::TraverseEdges(std::function<bool(const sm::vec3& begin, const sm::vec3& end)> func, const std::string& group_name) const
{
    if (!m_node) {
        return;
    }

    if (m_node->HasSharedComp<n3::CompModel>())
    {
        auto brush_model = GetBrushModel();
        assert(brush_model);
        auto& brushes = brush_model->GetBrushes();
        bool stop = false;
        for (auto& brush : brushes)
        {
            auto& polyhedron = brush.impl->GetHalfedge();
            auto first_edge = polyhedron->GetEdges().Head();
            auto curr_edge = first_edge;
            do {
                if (!func(curr_edge->vert->position, curr_edge->next->vert->position)) {
                    stop = true;
                }
                curr_edge = curr_edge->linked_next;
            } while (!stop && curr_edge != first_edge);
            if (stop) {
                break;
            }
        }
    }
    else if (m_node->HasSharedComp<n3::CompShape>())
    {
        bool stop = false;

        auto shape = GetShape();
        auto type = shape->get_type();
        if (type == rttr::type::get<gs::Polyline3D>())
        {
            auto polyline = std::static_pointer_cast<gs::Polyline3D>(shape);
            auto& vertices = polyline->GetVertices();
            for (int i = 0, n = vertices.size() - 1; i < n && !stop; ++i) {
                if (!func(vertices[i], vertices[i + 1])) {
                    stop = true;
                }
            }
        }
    }
}

void Geometry::TraverseFaces(std::function<bool(pm3::Polytope& poly, size_t face_idx, bool& dirty)> func,
                             const std::string& group_name) const
{
    if (!m_node) {
        return;
    }
    if (!m_node->HasSharedComp<n3::CompModel>()) {
        return;
    }

    bool dirty = false;

    auto brush_model = GetBrushModel();
    assert(brush_model);
    auto& brushes = brush_model->GetBrushes();

    std::shared_ptr<Group> group = nullptr;
    if (!group_name.empty()) {
        group = QueryGroup(group_name);
    }
    if (group)
    {
        assert(group->type == GroupType::Face);
        assert(brushes.size() == 1);
        auto& brush = brushes[0];
        bool stop = false;
        for (auto& i : group->items)
        {
            bool d;
            if (!func(*brush.impl, i, d)) {
                stop = true;
            }
            if (d) {
                dirty = true;
            }
            if (stop) {
                break;
            }
        }
    }
    else
    {
        bool stop = false;
        for (auto& brush : brushes)
        {
            for (size_t i = 0, n = brush.impl->Faces().size(); i < n; ++i)
            {
                bool d;
                if (!func(*brush.impl, i, d)) {
                    stop = true;
                }
                if (d) {
                    dirty = true;
                }
                if (stop) {
                    break;
                }
            }
            if (stop) {
                break;
            }
        }
    }

    if (dirty) {
        UpdateModel(*brush_model);
    }
}

void Geometry::Combine(const Geometry& geo, size_t face_offset)
{
    assert(geo.m_type == DataType::Brush);
    for (auto& group : geo.m_groups)
    {
        auto itr = m_groups.find(group.second->name);
        assert(itr == m_groups.end());
        assert(group.second->type == Geometry::GroupType::Face);
        auto new_group = std::make_shared<Group>(*group.second);
        for (auto& i : new_group->items) {
            i += face_offset;
        }
        m_groups.insert({ new_group->name, new_group });
    }
}

void Geometry::AddGroup(const std::shared_ptr<Group>& group)
{
    auto itr = m_groups.insert({ group->name, group });
    assert(itr.second);
}

std::shared_ptr<Geometry::Group> Geometry::QueryGroup(const std::string& name) const
{
    auto itr = m_groups.find(name);
    return itr == m_groups.end() ? nullptr : itr->second;
}

//void Geometry::RenameGroup(const std::string& src, const std::string& dst)
//{
//    auto itr = m_groups.find(src);
//    assert(itr != m_groups.end() && itr->second->name == src);
//    auto group = itr->second;
//    m_groups.erase(itr);
//    group->name = dst;
//    AddGroup(group);
//}

void Geometry::UpdateModel(const model::BrushModel& brush_model) const
{
    assert(m_type == DataType::Brush);

    std::shared_ptr<model::Model> model =
        model::BrushBuilder::PolymeshFromBrush(brush_model);

    auto& cmodel = m_node->GetSharedComp<n3::CompModel>();
    cmodel.SetModel(model);

    auto& cmodel_inst = m_node->GetUniqueComp<n3::CompModelInst>();
    cmodel_inst.SetModel(model, 0);

    auto& caabb = m_node->GetUniqueComp<n3::CompAABB>();
    caabb.SetAABB(model->aabb);
}

void Geometry::StoreBrush(std::unique_ptr<model::BrushModel>& brush_model)
{
    auto& cmodel_inst = m_node->GetUniqueComp<n3::CompModelInst>();
    std::unique_ptr<model::ModelExtend> model_ext = std::move(brush_model);
    cmodel_inst.GetModel()->SetModelExt(model_ext);
}

model::BrushModel* Geometry::GetBrushModel() const
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
Geometry::GetShape() const
{
    if (m_type != DataType::Shape) {
        return nullptr;
    }

    assert(m_node->HasSharedComp<n3::CompShape>());
    auto& cshape = m_node->GetSharedComp<n3::CompShape>();
    return cshape.GetShape();
}

void Geometry::CopyGroups(const Geometry& geo)
{
    m_groups.clear();
    for (auto& itr : geo.m_groups) {
        m_groups.insert({ itr.first, std::make_shared<Group>(*itr.second) });
    }
}

}