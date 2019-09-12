#include "utility.h"

#include <everything/Everything.h>
#include <everything/Node.h>
#include <everything/GeometryImpl.h>

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
    evt::Everything::Instance();
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

sm::vec3 get_point_pos(const evt::NodePtr& node, size_t idx)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);

    auto& pts = geo->GetAttr().GetPoints();
    REQUIRE(idx < pts.size());
    return pts[idx]->pos;
}

sm::vec3 get_vertex_pos(const evt::NodePtr& node, size_t idx)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);

    auto& vts = geo->GetAttr().GetVertices();
    REQUIRE(idx < vts.size());
    return vts[idx]->point->pos;
}

void get_face_pos(const evt::NodePtr& node, size_t idx, std::function<void(const sm::vec3&)> func)
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

void check_aabb(const evt::NodePtr& node, const sm::vec3& min, const sm::vec3& max)
{
    auto sn = node->GetGeometry()->GetNode();
    auto& caabb = sn->GetUniqueComp<n3::CompAABB>();
    auto& aabb = caabb.GetAABB();

    REQUIRE(aabb.Min()[0] == Approx(min.x));
    REQUIRE(aabb.Min()[1] == Approx(min.y));
    REQUIRE(aabb.Min()[2] == Approx(min.z));
    REQUIRE(aabb.Max()[0] == Approx(max.x));
    REQUIRE(aabb.Max()[1] == Approx(max.y));
    REQUIRE(aabb.Max()[2] == Approx(max.z));
}

void check_point(const evt::NodePtr& node, size_t idx, const sm::vec3& pos)
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

void check_vertex(const evt::NodePtr& node, size_t idx, const sm::vec3& pos)
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

void check_points_num(const evt::NodePtr& node, size_t num)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);
    REQUIRE(num == geo->GetAttr().GetPoints().size());
}

void check_vertices_num(const evt::NodePtr& node, size_t num)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);
    REQUIRE(num == geo->GetAttr().GetVertices().size());
}

void check_edges_num(const evt::NodePtr& node, size_t num)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);

    auto& brushes = geo->GetBrushModel()->GetBrushes();
    REQUIRE(brushes.size() == 1);
    auto& brush = brushes[0];
    REQUIRE(num == brush.impl->GetGeometry()->GetEdges().Size());
}

void check_faces_num(const evt::NodePtr& node, size_t num)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);
    REQUIRE(num == geo->GetAttr().GetPrimtives().size());
}

void check_group_num(const evt::NodePtr& node, const std::string& name, size_t num)
{
    auto geo = node->GetGeometry();
    REQUIRE(geo != nullptr);
    auto group = geo->GetGroup().Query(name);
    REQUIRE(group != nullptr);
    REQUIRE(group->items.size() == num);
}

void check_prop(const evt::NodePtr& node, const std::string& key, const evt::Variable& val)
{
    auto find = node->GetProps().Query(key);
    REQUIRE(find.type == val.type);
    if (find.type == evt::VariableType::Invalid) {
        return;
    }
    switch (val.type)
    {
    case evt::VariableType::Bool:
        REQUIRE(find.b == val.b);
        break;
    case evt::VariableType::Int:
        REQUIRE(find.i == val.i);
        break;
    case evt::VariableType::Float:
        REQUIRE(find.f == Approx(val.f));
        break;
    case evt::VariableType::Float3:
    {
        auto v0 = static_cast<const float*>(find.p);
        auto v1 = static_cast<const float*>(val.p);
        REQUIRE(v0[0] == v1[0]);
        REQUIRE(v0[1] == v1[1]);
        REQUIRE(v0[2] == v1[2]);
    }
        break;
    case evt::VariableType::String:
        REQUIRE(strcmp(static_cast<const char*>(find.p),
            static_cast<const char*>(val.p)) == 0);
        break;
    default:
        assert(0);
    }
}

}