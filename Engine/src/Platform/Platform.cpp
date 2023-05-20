#include "depch.h"
#include "Platform.h"

#include "Engine/Renderer/RendererAPI.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	namespace Platform
	{
		float GetTime()
		{
			return (float)glfwGetTime();
		}
	}
}