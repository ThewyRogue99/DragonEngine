#include "depch.h"

#include "Renderer.h"
#include "RendererAPI.h"

#include "Engine/Core/Log.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

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