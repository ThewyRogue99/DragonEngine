#include "depch.h"
#include "Platform.h"

#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/RendererAPI.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	namespace Platform
	{
		float GetTime()
		{
			switch (Renderer::GetAPI())
			{
				case RendererAPI::API::OpenGL: return (float)glfwGetTime();
				default:
					return 0.f;
			}
		}
	}
}