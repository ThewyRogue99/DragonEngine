#include "depch.h"
#include "Texture.h"

#include "Engine/Debug/Assert.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Engine
{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, uint32_t channels)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width, height, channels);
		default:
		{
			DE_ASSERT(false, "Unknown Renderer API");
			return nullptr;
		}
		}
	}
}