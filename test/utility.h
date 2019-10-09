#pragma once

#include <sop/typedef.h>
#include <sop/GeoAttribute.h>

#include <SM_Vector.h>

#include <string>
#include <functional>

namespace sop { class Variable; }

namespace test
{

void init();

sm::vec3 get_point_pos(const sop::NodePtr& node, size_t idx);
sm::vec3 get_vertex_pos(const sop::NodePtr& node, size_t idx);
void get_face_pos(const sop::NodePtr& node, size_t idx, std::function<void(const sm::vec3&)> func);

void check_aabb(const sop::NodePtr& node, const sm::vec3& min, const sm::vec3& max);
void check_point(const sop::NodePtr& node, size_t idx, const sm::vec3& pos);
void check_vertex(const sop::NodePtr& node, size_t idx, const sm::vec3& pos);

void check_points_num(const sop::NodePtr& node, size_t num);
void check_vertices_num(const sop::NodePtr& node, size_t num);
void check_faces_num(const sop::NodePtr& node, size_t num);

void check_halfedge_vertices_num(const sop::NodePtr& node, size_t num);
void check_halfedge_edges_num(const sop::NodePtr& node, size_t num);
void check_halfedge_faces_num(const sop::NodePtr& node, size_t num);

void check_attr_count(const sop::NodePtr& node, sop::GeoAttrClass type,
    const std::string& name, size_t num);
void check_attr_value(const sop::NodePtr& node, sop::GeoAttrClass type,
    const std::string& name, size_t idx, const sop::Variable& var);

void check_group_num(const sop::NodePtr& node, const std::string& name, size_t num);

void check_prop(const sop::NodePtr& node, const std::string& key, const sop::Variable& val);

}