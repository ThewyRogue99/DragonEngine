#include "depch.h"
#include "OpenGLContext.h"

#include "Engine/Debug/Instrumentor.h"
#include "Engine/Debug/Assert.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	OpenGLContext::OpenGLContext(GLFWwindow* WindowHandle) : WindowHandle(WindowHandle)
	{
		DE_ASSERT(WindowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(WindowHandle);
	}

	void OpenGLContext::SwapBuffers()
	{
		DE_PROFILE_FUNCTION();

		glfwSwapBuffers(WindowHandle);
	}
}