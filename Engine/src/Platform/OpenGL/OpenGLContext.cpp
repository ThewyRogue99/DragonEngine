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
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(WindowHandle);
	}
}