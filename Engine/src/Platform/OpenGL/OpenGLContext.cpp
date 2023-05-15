#include "depch.h"
#include "OpenGLContext.h"

#include "Engine/Debug/Instrumentor.h"
#include "Engine/Debug/Assert.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Engine
{
	OpenGLContext::OpenGLContext(GLFWwindow* WindowHandle) : WindowHandle(WindowHandle)
	{
		DE_ASSERT(WindowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		DE_ASSERT(status, "Could not initialize GLAD!");

		DE_INFO(OpenGLContext, "OpenGL Renderer: {0} {1}", (char*)glGetString(GL_VENDOR), (char*)glGetString(GL_RENDERER));
		DE_INFO(OpenGLContext, "Vendor: {0}", (char*)glGetString(GL_VENDOR));
		DE_INFO(OpenGLContext, "Renderer: {0}", (char*)glGetString(GL_RENDERER));
		DE_INFO(OpenGLContext, "Version: {0}", (char*)glGetString(GL_VERSION));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLContext::SwapBuffers()
	{
		DE_PROFILE_FUNCTION();

		glfwSwapBuffers(WindowHandle);
	}
}