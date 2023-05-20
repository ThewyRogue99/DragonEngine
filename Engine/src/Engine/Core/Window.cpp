#include "depch.h"
#include "Window.h"

#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLWindow.h"
#include "Platform/Vulkan/VulkanWindow.h"

namespace Engine
{
	Ref<Window> Window::Create(const WindowProps& props)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLWindow>(props);
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanWindow>(props);
		default:
			return nullptr;
		}
	}
}
