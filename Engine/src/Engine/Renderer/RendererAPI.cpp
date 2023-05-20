#include "depch.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Engine
{
	Ref<RendererAPI> RendererAPI::Create(API api)
	{
		switch (api)
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLRendererAPI>();
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanRendererAPI>();
		default:
			return nullptr;
		}
	}
}