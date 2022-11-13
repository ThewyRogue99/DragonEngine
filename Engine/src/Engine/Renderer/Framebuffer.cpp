#include "depch.h"
#include "Framebuffer.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Engine
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& props)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(props);
			default:
			{
				DE_CORE_ASSERT(false, "Unknown Renderer API");
				return nullptr;
			}
		}
	}
}