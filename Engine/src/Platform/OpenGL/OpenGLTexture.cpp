#include "depch.h"
#include "OpenGLTexture.h"

#include "Engine/Debug/Assert.h"

#include <glad/glad.h>

namespace Engine
{
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, uint32_t channels)
		: Width(width), Height(height)
	{
		switch (channels)
		{
			case 3:
			{
				InternalFormat = GL_RGB8;
				DataFormat = GL_RGB;
			} break;
			case 4:
			{
				InternalFormat = GL_RGBA8;
				DataFormat = GL_RGBA;
			} break;
			default:
			{
				DE_ASSERT(false, "Failed to allocate texture memory: Data format is not supported!");
			} return;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &TextureID);

		glBindTexture(GL_TEXTURE_2D, TextureID);

		glTextureStorage2D(TextureID, 1, InternalFormat, Width, Height);

		glTextureParameteri(TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &TextureID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = ((DataFormat == GL_RGBA) ? 4 : 3);
		DE_ASSERT(size == Width * Height * bpp, "Data must be entire texture!");

		glTextureSubImage2D(TextureID, 0, 0, 0, Width, Height, DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, TextureID);
	}

	bool OpenGLTexture2D::operator==(Texture2D& texture)
	{
		return TextureID == texture.GetRendererID();
	}
}