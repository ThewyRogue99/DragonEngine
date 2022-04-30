#pragma once

#include "Engine/Core.h"
#include "Engine/Renderer/Texture.h"

namespace Engine
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(std::string& path);
		OpenGLTexture2D(const char* path);

		virtual ~OpenGLTexture2D();

		inline virtual uint32_t GetWidth() const override { return Width; }
		inline virtual uint32_t GetHeight() const override { return Height; }

		virtual void Bind(uint32_t slot = 0) const override;

		void Load();

	private:
		std::string Path;

		uint32_t Width = 0, Height = 0;

		uint32_t TextureID = 0;
	};
}
