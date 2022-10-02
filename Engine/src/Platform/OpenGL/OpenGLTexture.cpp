#include "depch.h"
#include "OpenGLTexture.h"

#include "Engine/Types/Types.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Engine
{
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: Width(width), Height(height)
	{
		DataFormat = GL_RGBA, InternalFormat = GL_RGBA8;

		glCreateTextures(GL_TEXTURE_2D, 1, &TextureID);
		glTextureStorage2D(TextureID, 1, InternalFormat, Width, Height);

		glTextureParameteri(TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const wchar_t* path) : Path(path)
	{
		stbi_set_flip_vertically_on_load(true);

		std::string path_str = TypeUtils::FromUTF16(path);

		int width, height, channels;
		unsigned char* data = stbi_load(path_str.c_str(), &width, &height, &channels, 0);
		DE_CORE_ASSERT(data, "Failed to load texture image");

		Width = width;
		Height = height;

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
			DE_CORE_ASSERT(false, "Failed to load texture image: Image format is not supported!");
			stbi_image_free(data);
		} return;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &TextureID);
		glTextureStorage2D(TextureID, 1, InternalFormat, Width, Height);

		glTextureParameteri(TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(TextureID, 0, 0, 0, Width, Height, DataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &TextureID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = ((DataFormat == GL_RGBA) ? 4 : 3);
		DE_CORE_ASSERT(size == Width * Height * bpp, "Data must be entire texture!");

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