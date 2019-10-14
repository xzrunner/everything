#include "sop/node/PolyExtrude.h"
#include "sop/GeometryImpl.h"
#include "sop/NodeHelper.h"

#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
#include <model/BrushModel.h>

namespace sop
{
namespace node
{

void PolyExtrude::Execute(Evaluator& eval)
{
    m_geo_impl.reset();

    auto prev_geo = NodeHelper::GetInputGeo(*this, 0);
    if (!prev_geo) {
        return;
    }

    m_geo_impl = std::make_shared<GeometryImpl>(*prev_geo);
    auto brush_model = m_geo_impl->GetBrushModel();
    if (!brush_model) {
        return;
    }

    bool dirty = false;
    auto& prims = m_geo_impl->GetAttr().GetPrimtives();
    auto group = m_geo_impl->GetGroup().Query(m_group_name);
    std::vector<std::shared_ptr<GeoAttribute::Primitive>> edited_prims;
    if (group)
    {
        assert(group->GetType() == GroupType::Primitives);
        for (auto& i : group->GetItems()) {
            edited_prims.push_back(prims[i]);
        }
    }
    else
    {
        edited_prims = prims;
    }

    std::vector<he::Face*> new_faces[he::Polyhedron::ExtrudeMaxCount];
    if (ExtrudeFace(edited_prims, new_faces))
    {
        m_geo_impl->UpdateByBrush(*brush_model);

        // update group
        if (!m_front_group.empty()) {
            AddToGroup(m_front_group, new_faces[he::Polyhedron::ExtrudeFront]);
        }
        if (!m_back_group.empty()) {
            AddToGroup(m_back_group, new_faces[he::Polyhedron::ExtrudeBack]);
        }
        if (!m_side_group.empty()) {
            AddToGroup(m_side_group, new_faces[he::Polyhedron::ExtrudeSide]);
        }
    }
}

void PolyExtrude::SetGroupName(const std::string& name)
{
    if (m_group_name == name) {
        return;
    }

    m_group_name = name;

    SetDirty(true);
}

void PolyExtrude::SetDistance(float dist)
{
    if (m_distance == dist) {
        return;
    }

    m_distance = dist;

    SetDirty(true);
}

void PolyExtrude::SetOutputFront(bool output)
{
    if (m_output_front == output) {
        return;
    }

    m_output_front = output;

    SetDirty(true);
}

void PolyExtrude::SetOutputBack(bool output)
{
    if (m_output_back == output) {
        return;
    }

    m_output_back = output;

    SetDirty(true);
}

void PolyExtrude::SetOutputSide(bool output)
{
    if (m_output_side == output) {
        return;
    }

    m_output_side = output;

    SetDirty(true);
}

void PolyExtrude::SetFrontGroupName(const std::string& name)
{
    if (m_front_group == name) {
        return;
    }

    m_front_group = name;

    SetDirty(true);
}

void PolyExtrude::SetBackGroupName(const std::string& name)
{
    if (m_back_group == name) {
        return;
    }

    m_back_group = name;

    SetDirty(true);
}

void PolyExtrude::SetSideGroupName(const std::string& name)
{
    if (m_side_group == name) {
        return;
    }

    m_side_group = name;

    SetDirty(true);
}

bool PolyExtrude::ExtrudeFace(const std::vector<std::shared_ptr<GeoAttribute::Primitive>>& prims,
                              std::vector<he::Face*>* new_faces)
{
    auto brush_model = m_geo_impl->GetBrushModel();
    if (!brush_model) {
        return false;
    }

    auto& brushes = brush_model->GetBrushes();
    std::vector<std::vector<he::TopoID>> face_ids(brushes.size());
    for (auto& prim : prims) {
        assert(prim->prim_id >= 0 && prim->prim_id < brushes.size());
        face_ids[prim->prim_id].push_back(prim->topo_id);
    }

    bool dirty = false;

    bool create_face[he::Polyhedron::ExtrudeMaxCount];
    create_face[he::Polyhedron::ExtrudeFront] = m_output_front;
    create_face[he::Polyhedron::ExtrudeBack]  = m_output_back;
    create_face[he::Polyhedron::ExtrudeSide]  = m_output_side;

    for (int i = 0, n = brushes.size(); i < n; ++i)
    {
        if (face_ids[i].empty()) {
            continue;
        }

        auto poly = brushes[i].impl;
        if (!poly) {
            continue;
        }
        auto he_poly = poly->GetGeometry();
        if (!he_poly) {
            continue;
        }

        std::vector<he::Face*> faces[he::Polyhedron::ExtrudeMaxCount];
        if (!he_poly->Extrude(m_distance, face_ids[i], create_face, faces)) {
            continue;
        }

        for (size_t i = 0; i < he::Polyhedron::ExtrudeMaxCount; ++i) {
            for (auto& f : faces[i]) {
                new_faces[i].push_back(f);
            }
        }

        poly->BuildFromGeo();

        dirty = true;
    }

    return dirty;
}

void PolyExtrude::AddToGroup(const std::string& group_name, const std::vector<he::Face*>& faces)
{
    std::set<size_t> face_ids;
    for (auto& f : faces) {
        face_ids.insert(f->ids.UID());
    }

    std::vector<size_t> items;
    items.reserve(faces.size());
    auto& prims = m_geo_impl->GetAttr().GetPrimtives();
    for (size_t i = 0, n = prims.size(); i < n; ++i) {
        if (face_ids.find(prims[i]->topo_id.UID()) != face_ids.end()) {
            items.push_back(i);
        }
    }

    auto& group_mgr = m_geo_impl->GetGroup();
    auto group = group_mgr.Query(group_name);
    if (group)
    {
        assert(group->GetType() == GroupType::Primitives);
        for (auto& i : items) {
            group->Add(i);
        }
    }
    else
    {
        group = std::make_shared<Group>();
        group->SetName(group_name);
        group->SetType(GroupType::Primitives);
        group->SetItems(items);
        group_mgr.Add(group, GroupMerge::Replace);
    }
}

}
}