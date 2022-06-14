#include "depch.h"
#include "UniformBuffer.h"

#include "Engine/Renderer/Renderer2D.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Engine
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL:
				return CreateRef<OpenGLUniformBuffer>(size, binding);
			default:
				DE_CORE_ASSERT(false, "Unknown Renderer API!");
				return nullptr;
		}
	}
}