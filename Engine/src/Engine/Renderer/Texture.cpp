#include "depch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Engine/Core/Log.h"
#include "Platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>

namespace Engine
{
	Ref<Texture2D> Texture2D::Create(const char* path)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path);
			default:
			{
				DE_CORE_ASSERT(false, "Unknown Renderer API");
				return nullptr;
			}
		}
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width, height);
		default:
		{
			DE_CORE_ASSERT(false, "Unknown Renderer API");
			return nullptr;
		}
		}
	}
}