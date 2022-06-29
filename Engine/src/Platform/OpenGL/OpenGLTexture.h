#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Renderer/Texture.h"

namespace Engine
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const wchar_t* path);

		virtual ~OpenGLTexture2D();

		inline virtual uint32_t GetWidth() const override { return Width; }
		inline virtual uint32_t GetHeight() const override { return Height; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual uint32_t GetRendererID() const override { return TextureID; };

		virtual bool operator==(Texture2D& texture) override;

	private:
		const wchar_t* Path = nullptr;

		uint32_t Width = 0, Height = 0;

		uint32_t TextureID = 0;

		unsigned int InternalFormat = 0, DataFormat = 0;
	};
}
