#include "depch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Engine
{
	OpenGLTexture2D::OpenGLTexture2D(std::string& path) : Path(path)
	{

	}

	OpenGLTexture2D::OpenGLTexture2D(const char* path) : Path(path)
	{

	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &TextureID);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, TextureID);
	}

	void OpenGLTexture2D::Load()
	{
		stbi_set_flip_vertically_on_load(true);

		int width, height, channels;
		unsigned char* data = stbi_load(Path.c_str(), &width, &height, &channels, 0);
		DE_CORE_ASSERT(data, "Failed to load texture image");

		Width = width;
		Height = height;

		glCreateTextures(GL_TEXTURE_2D, 1, &TextureID);
		glTextureStorage2D(TextureID, 1, GL_RGB8, Width, Height);

		glTextureParameteri(TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(TextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(TextureID, 0, 0, 0, Width, Height, GL_RGB, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
}