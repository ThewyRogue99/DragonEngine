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
			case RendererAPI::API::OpenGL:
			{
				auto ref = std::make_shared<OpenGLTexture2D>(path);
				ref->Load();

				return ref;
			}
			default:
			{
				DE_CORE_ASSERT(false, "Unknown Renderer API");
				return nullptr;
			}
		}
	}

	Ref<Texture2D> Texture2D::Create(std::string& path)
	{
		return Create(path.c_str());
	}
}