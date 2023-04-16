#include "depch.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

#include "Engine/Debug/Assert.h"
#include "RendererAPI.h"

namespace Engine
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexArray>();
			default:
			{
				DE_ASSERT(false, "Unknown Renderer API");
				return nullptr;
			}
		}
	}
}