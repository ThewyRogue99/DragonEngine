#include "depch.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

#include "Renderer.h"

namespace Engine
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexArray>();
			default:
			{
				DE_CORE_ASSERT(false, "Unknown Renderer API");
				return nullptr;
			}
		}
	}
}