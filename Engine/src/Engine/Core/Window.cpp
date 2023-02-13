#include "depch.h"
#include "Window.h"

#include "Engine/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLWindow.h"

namespace Engine
{
	Ref<Window> Window::Create(const WindowProps& props)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLWindow>(props);
		default:
			return nullptr;
		}
	}
}
