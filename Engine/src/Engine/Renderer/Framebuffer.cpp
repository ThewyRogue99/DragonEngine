#include "depch.h"
#include "Framebuffer.h"

#include "Renderer.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Engine
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& props)
	{
		switch (Renderer::GetAPI())
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