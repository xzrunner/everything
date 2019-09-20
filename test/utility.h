#pragma once

#include <everything/typedef.h>
#include <everything/GeoAttribute.h>

#include <SM_Vector.h>

#include <string>
#include <functional>

namespace evt { class Variable; }

namespace test
{

void init();

sm::vec3 get_point_pos(const evt::NodePtr& node, size_t idx);
sm::vec3 get_vertex_pos(const evt::NodePtr& node, size_t idx);
void get_face_pos(const evt::NodePtr& node, size_t idx, std::function<void(const sm::vec3&)> func);

void check_aabb(const evt::NodePtr& node, const sm::vec3& min, const sm::vec3& max);
void check_point(const evt::NodePtr& node, size_t idx, const sm::vec3& pos);
void check_vertex(const evt::NodePtr& node, size_t idx, const sm::vec3& pos);

void check_points_num(const evt::NodePtr& node, size_t num);
void check_vertices_num(const evt::NodePtr& node, size_t num);
void check_faces_num(const evt::NodePtr& node, size_t num);

void check_halfedge_vertices_num(const evt::NodePtr& node, size_t num);
void check_halfedge_edges_num(const evt::NodePtr& node, size_t num);
void check_halfedge_faces_num(const evt::NodePtr& node, size_t num);

void check_attr_count(const evt::NodePtr& node, evt::GeoAttrType type,
    const std::string& name, size_t num);
void check_attr_value(const evt::NodePtr& node, evt::GeoAttrType type,
    const std::string& name, size_t idx, const evt::Variable& var);

void check_group_num(const evt::NodePtr& node, const std::string& name, size_t num);

void check_prop(const evt::NodePtr& node, const std::string& key, const evt::Variable& val);

}