#include "utility.h"

#include <sop/SOP.h>
#include <sop/Node.h>
#include <sop/GeometryImpl.h>

#include <unirender/gl/RenderContext.h>
#include <unirender/Blackboard.h>
#include <halfedge/Polyhedron.h>
#include <polymesh3/Geometry.h>
#include <model/BrushBuilder.h>
#include <painting3/AABB.h>
#include <renderpipeline/RenderMgr.h>
#include <node0/SceneNode.h>
#include <node3/CompAABB.h>

#include <gl/glew.h>
#include <glfw3.h>
#include <catch/catch.hpp>

namespace
{

void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

bool InitGL()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(100, 100, "rotate-crop", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	//// Initialize GLEW to setup the OpenGL Function pointers
	//if (glewInit() != GLEW_OK) {
	//	return -1;
	//}

	return true;
}

void InitRender()
{
    auto ur_rc = std::make_shared<ur::gl::RenderContext>(4096, [&](ur::RenderContext& ctx) {
        ctx.EnableFlushCB(false);
        rp::RenderMgr::Instance()->Flush();
        ctx.EnableFlushCB(true);
    });
    ur::Blackboard::Instance()->SetRenderContext(ur_rc);
}

void InitEverything()
{
    sop::SOP::Instance();
}

}

namespace test
{

void init()
{
    static bool inited = false;
    if (!inited)
    {
        InitGL();
        InitRender();
        InitEverything();

        inited = true;
    }
}

sm::vec3 get_point_pos(const sop::NodePtr& node, size_t idx)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);

    auto& pts = geo->GetAttr().GetPoints();
    REQUIRE(idx < pts.size());
    return pts[idx]->pos;
}

sm::vec3 get_vertex_pos(const sop::NodePtr& node, size_t idx)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);

    auto& vts = geo->GetAttr().GetVertices();
    REQUIRE(idx < vts.size());
    return vts[idx]->point->pos;
}

void get_face_pos(const sop::NodePtr& node, size_t idx, std::function<void(const sm::vec3&)> func)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);

    auto& prims = geo->GetAttr().GetPrimtives();
    REQUIRE(idx < prims.size());

    auto& prim = prims[idx];
    for (auto& v : prim->vertices) {
        func(v->point->pos);
    }
}

void check_aabb(const sop::NodePtr& node, const sm::vec3& min, const sm::vec3& max)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);

    auto sn = geo->GetNode();
    auto& caabb = sn->GetUniqueComp<n3::CompAABB>();
    auto& aabb = caabb.GetAABB();

    REQUIRE(aabb.Min()[0] == Approx(min.x));
    REQUIRE(aabb.Min()[1] == Approx(min.y));
    REQUIRE(aabb.Min()[2] == Approx(min.z));
    REQUIRE(aabb.Max()[0] == Approx(max.x));
    REQUIRE(aabb.Max()[1] == Approx(max.y));
    REQUIRE(aabb.Max()[2] == Approx(max.z));
}

void check_point(const sop::NodePtr& node, size_t idx, const sm::vec3& pos)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);

    auto& pts = geo->GetAttr().GetPoints();

    REQUIRE(idx < pts.size());
    auto& p = pts[idx]->pos;

    REQUIRE(p.x == Approx(pos.x));
    REQUIRE(p.y == Approx(pos.y));
    REQUIRE(p.z == Approx(pos.z));
}

void check_vertex(const sop::NodePtr& node, size_t idx, const sm::vec3& pos)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);

    auto& vts = geo->GetAttr().GetVertices();

    REQUIRE(idx < vts.size());
    auto& p = vts[idx]->point->pos;

    REQUIRE(p.x == Approx(pos.x));
    REQUIRE(p.y == Approx(pos.y));
    REQUIRE(p.z == Approx(pos.z));
}

void check_points_num(const sop::NodePtr& node, size_t num)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);
    REQUIRE(num == geo->GetAttr().GetPoints().size());
}

void check_vertices_num(const sop::NodePtr& node, size_t num)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);
    REQUIRE(num == geo->GetAttr().GetVertices().size());
}

void check_faces_num(const sop::NodePtr& node, size_t num)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);
    REQUIRE(num == geo->GetAttr().GetPrimtives().size());
}

void check_halfedge_vertices_num(const sop::NodePtr& node, size_t num)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);

    size_t n = 0;
    for (auto& b : geo->GetBrushModel()->GetBrushes()) {
        n += b.impl->GetGeometry()->GetVertices().Size();
    }
    REQUIRE(num == n);
}

void check_halfedge_edges_num(const sop::NodePtr& node, size_t num)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);

    size_t n = 0;
    for (auto& b : geo->GetBrushModel()->GetBrushes()) {
        n += b.impl->GetGeometry()->GetEdges().Size();
    }
    REQUIRE(num == n);
}

void check_halfedge_faces_num(const sop::NodePtr& node, size_t num)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);

    size_t n = 0;
    for (auto& b : geo->GetBrushModel()->GetBrushes()) {
        n += b.impl->GetGeometry()->GetFaces().Size();
    }
    REQUIRE(num == n);
}

void check_attr_count(const sop::NodePtr& node, sop::GeoAttrClass cls,
                      const std::string& name, size_t num)
{
    if (num == 0) {
        return;
    }

    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);
    auto& attr = geo->GetAttr();

    auto var = attr.QueryAttr(cls, name, 0);
    REQUIRE(var.type != sop::VarType::Invalid);
    switch (cls)
    {
    case sop::GeoAttrClass::Point:
        REQUIRE(attr.GetPoints().size() == num);
        break;
    case sop::GeoAttrClass::Vertex:
        REQUIRE(attr.GetVertices().size() == num);
        break;
    case sop::GeoAttrClass::Primitive:
        REQUIRE(attr.GetPrimtives().size() == num);
        break;
    case sop::GeoAttrClass::Detail:
        REQUIRE(1 == num);
        break;
    }
}

void check_attr_value(const sop::NodePtr& node, sop::GeoAttrClass cls,
                      const std::string& name, size_t idx, const sop::Variable& var)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);
    auto var2 = geo->GetAttr().QueryAttr(cls, name, idx);
    REQUIRE(var == var2);
}

void check_group_num(const sop::NodePtr& node, const std::string& name, size_t num)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);
    auto group = geo->GetGroup().Query(name);
    REQUIRE(group != nullptr);
    REQUIRE(group->Size() == num);
}

void check_group_type(const sop::NodePtr& node, const std::string& name, sop::GroupType type)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);
    auto group = geo->GetGroup().Query(name);
    REQUIRE(group != nullptr);
    REQUIRE(group->GetType() == type);
}

void check_prop(const sop::NodePtr& node, const std::string& key, const sop::Variable& val)
{
    auto find = node->GetProps().Query(key);
    REQUIRE(find.type == val.type);
    if (find.type == sop::VarType::Invalid) {
        return;
    }
    switch (val.type)
    {
    case sop::VarType::Bool:
        REQUIRE(find.b == val.b);
        break;
    case sop::VarType::Int:
        REQUIRE(find.i == val.i);
        break;
    case sop::VarType::Float:
        REQUIRE(find.f == Approx(val.f));
        break;
    case sop::VarType::Float3:
    {
        auto v0 = static_cast<const float*>(find.p);
        auto v1 = static_cast<const float*>(val.p);
        REQUIRE(v0[0] == v1[0]);
        REQUIRE(v0[1] == v1[1]);
        REQUIRE(v0[2] == v1[2]);
    }
        break;
    case sop::VarType::String:
        REQUIRE(strcmp(static_cast<const char*>(find.p),
            static_cast<const char*>(val.p)) == 0);
        break;
    default:
        assert(0);
    }
}

}