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
			switch (RendererAPI::GetAPI())
			{
				case RendererAPI::API::OpenGL: return (float)glfwGetTime();
				default:
					return 0.f;
			}
		}
	}
}