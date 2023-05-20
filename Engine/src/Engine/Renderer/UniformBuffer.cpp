#include "depch.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

#include "Engine/Debug/Assert.h"

#include "Renderer.h"

namespace Engine
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL:
			{
				return CreateRef<OpenGLUniformBuffer>(size, binding);
			}
			default:
			{
				DE_ASSERT(false, "Unknown Renderer API!");
				return nullptr;
			}
		}
	}
}