#include "depch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Engine
{
	OpenGLContext::OpenGLContext(GLFWwindow* WindowHandle) : WindowHandle(WindowHandle)
	{
		DE_CORE_ASSERT(WindowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		DE_CORE_ASSERT(status, "Could not initialize GLAD!");

		DE_CORE_INFO("OpenGL Renderer: {0} {1}", (char*)glGetString(GL_VENDOR), (char*)glGetString(GL_RENDERER));
		DE_CORE_INFO("\tVendor: {0}", (char*)glGetString(GL_VENDOR));
		DE_CORE_INFO("\tRenderer: {0}", (char*)glGetString(GL_RENDERER));
		DE_CORE_INFO("\tVersion: {0}", (char*)glGetString(GL_VERSION));

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