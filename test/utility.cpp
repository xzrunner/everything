#include "utility.h"

#include <unirender/gl/RenderContext.h>
#include <unirender/Blackboard.h>
#include <painting3/AABB.h>
#include <renderpipeline/RenderMgr.h>

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

}

namespace test
{

void init()
{
    static bool inited = false;
    if (!inited) {
        InitGL();
        InitRender();
        inited = true;
    }
}

}