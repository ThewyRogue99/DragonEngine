#include "depch.h"
#include "UniformBuffer.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Engine
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (RendererAPI::GetAPI())
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