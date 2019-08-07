#include "everything/TreeContext.h"
#include "everything/node/Box.h"

#include <SM_Vector.h>
#include <polymesh3/Brush.h>
#include <model/Model.h>
#include <model/ModelExtend.h>
#include <model/BrushModel.h>
#include <node0/SceneNode.h>
#include <node3/CompModelInst.h>
#include <node3/CompAABB.h>
#include <unirender/gl/RenderContext.h>
#include <unirender/Blackboard.h>
#include <painting3/AABB.h>
#include <renderpipeline/RenderMgr.h>

#include <catch/catch.hpp>
#include <gl/glew.h>
#include <glfw3.h>

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

void init()
{
    static bool inited = false;
    if (!inited) {
        InitGL();
        InitRender();
        inited = true;
    }
}

model::BrushModel* GetBrushModel(const n0::SceneNodePtr& node)
{
    auto& cmodel_inst = node->GetUniqueComp<n3::CompModelInst>();
    auto& ext = cmodel_inst.GetModel()->GetModelExt();
    REQUIRE(ext);
    REQUIRE(ext->Type() == model::EXT_BRUSH);
    model::BrushModel* brush_model = static_cast<model::BrushModel*>(ext.get());
    REQUIRE(brush_model);
    return brush_model;
}

}

TEST_CASE("box")
{
    init();

    auto box = std::make_shared<evt::node::Box>();

    const sm::vec3 sz(2, 3, 4);
    auto h_sz = sz * 0.5f;
    box->SetSize(sz);
    box->Execute(evt::TreeContext());
    auto node = box->GetSceneNode();
    auto brush_model = GetBrushModel(node);

    auto& brushes = brush_model->GetBrushes();
    REQUIRE(brushes.size() == 1);
    auto& brush = brushes[0];
    REQUIRE(brush.impl->faces.size() == 6);
    REQUIRE(brush.impl->vertices.size() == 8);

    auto& caabb = node->GetUniqueComp<n3::CompAABB>();
    auto& aabb = caabb.GetAABB();
    REQUIRE(aabb.Min()[0] == -h_sz.x);
    REQUIRE(aabb.Min()[1] == -h_sz.y);
    REQUIRE(aabb.Min()[2] == -h_sz.z);
    REQUIRE(aabb.Max()[0] == h_sz.x);
    REQUIRE(aabb.Max()[1] == h_sz.y);
    REQUIRE(aabb.Max()[2] == h_sz.z);

    const sm::vec3 off(10, 11, 12);
    box->SetCenter(off);
    box->Execute(evt::TreeContext());
    REQUIRE(aabb.Min()[0] == -h_sz.x + off.x);
    REQUIRE(aabb.Min()[1] == -h_sz.y + off.y);
    REQUIRE(aabb.Min()[2] == -h_sz.z + off.z);
    REQUIRE(aabb.Max()[0] == h_sz.x + off.x);
    REQUIRE(aabb.Max()[1] == h_sz.y + off.y);
    REQUIRE(aabb.Max()[2] == h_sz.z + off.z);

    const sm::vec3 scale(5, 6, 7);
    box->SetScale(scale);
    box->Execute(evt::TreeContext());
    REQUIRE(aabb.Min()[0] == -h_sz.x * scale.x + off.x);
    REQUIRE(aabb.Min()[1] == -h_sz.y * scale.y + off.y);
    REQUIRE(aabb.Min()[2] == -h_sz.z * scale.z + off.z);
    REQUIRE(aabb.Max()[0] == h_sz.x * scale.x + off.x);
    REQUIRE(aabb.Max()[1] == h_sz.y * scale.y + off.y);
    REQUIRE(aabb.Max()[2] == h_sz.z * scale.z + off.z);
}