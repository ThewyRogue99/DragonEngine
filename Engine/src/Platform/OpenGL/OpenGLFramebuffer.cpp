#include "depch.h"
#include "OpenGLFramebuffer.h"

#include "Engine/Debug/Assert.h"

#include <glad/glad.h>

namespace Engine
{
	namespace Utils
	{
		static GLenum TextureTarget(bool IsMultiSample)
		{
			return IsMultiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool IsMultiSample, uint32_t* OutID, uint32_t Count)
		{
			glCreateTextures(TextureTarget(IsMultiSample), Count, OutID);
		}

		static void BindTexture(bool IsMultiSample, uint32_t ID)
		{
			glBindTexture(TextureTarget(IsMultiSample), ID);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;

			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;

			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
					return true;
				default:
					return false;
			}
		}

		static GLenum GetGLFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case Engine::FramebufferTextureFormat::RGBA8:
					return GL_RGBA8;
				case Engine::FramebufferTextureFormat::RED_INTEGER:
					return GL_RED_INTEGER;
				default:
				{
					DE_ASSERT(false, "Unsupported texture format");
					return GL_NONE;
				}
			}
		}
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& Specification) : BufferSpecification(Specification)
	{
		for (auto& spec : BufferSpecification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
				ColorAttachmentSpecifications.emplace_back(spec);
			else
				DepthAttachmentSpecification = spec;
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &RendererID);
		glDeleteTextures((GLsizei)ColorAttachments.size(), ColorAttachments.data());
		glDeleteTextures(1, &DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (RendererID)
		{
			glDeleteFramebuffers(1, &RendererID);
			glDeleteTextures((GLsizei)ColorAttachments.size(), ColorAttachments.data());
			glDeleteTextures(1, &DepthAttachment);

			ColorAttachments.clear();
			DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &RendererID);

		Bind();

		// Attachments

		bool multisample = BufferSpecification.Samples > 1;

		if (ColorAttachmentSpecifications.size() > 0)
		{
			ColorAttachments.resize(ColorAttachmentSpecifications.size());
			Utils::CreateTextures(multisample, ColorAttachments.data(), (uint32_t)ColorAttachments.size());

			for (size_t i = 0; i < ColorAttachmentSpecifications.size(); i++)
			{
				Utils::BindTexture(multisample, ColorAttachments[i]);

				switch (ColorAttachmentSpecifications[i].TextureFormat)
				{
					case FramebufferTextureFormat::RGBA8:
					{
						Utils::AttachColorTexture(
							ColorAttachments[i],
							BufferSpecification.Samples,
							GL_RGBA8,
							GL_RGBA,
							BufferSpecification.Width,
							BufferSpecification.Height,
							(int)i
						);
					} break;
					case FramebufferTextureFormat::RED_INTEGER:
					{
						Utils::AttachColorTexture(
							ColorAttachments[i],
							BufferSpecification.Samples,
							GL_R32I,
							GL_RED_INTEGER,
							BufferSpecification.Width,
							BufferSpecification.Height,
							(int)i
						);
					} break;
					default:
						break;
				}
			}
		}

		if (DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			Utils::CreateTextures(multisample, &DepthAttachment, 1);
			Utils::BindTexture(multisample, DepthAttachment);

			switch (DepthAttachmentSpecification.TextureFormat)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
				{
					Utils::AttachDepthTexture(
						DepthAttachment,
						BufferSpecification.Samples,
						GL_DEPTH24_STENCIL8,
						GL_DEPTH_STENCIL_ATTACHMENT,
						BufferSpecification.Width,
						BufferSpecification.Height
					);
				} break;
				default:
					break;
			}
		}

		if (ColorAttachments.size() > 1)
		{
			DE_ASSERT(ColorAttachments.size() <= 4, "Engine only supports 4 color attachments!");

			GLenum buffers[4] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2,
				GL_COLOR_ATTACHMENT3
			};

			glDrawBuffers((GLsizei)ColorAttachments.size(), buffers);
		}
		else if (ColorAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}

		Unbind();

		DE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");
	}

	void OpenGLFramebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, RendererID);
	}

	void OpenGLFramebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t Width, uint32_t Height)
	{
		BufferSpecification.Width = Width;
		BufferSpecification.Height = Height;

		Invalidate();

		Bind();
		glViewport(0, 0, BufferSpecification.Width, BufferSpecification.Height);
		Unbind();
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t AttachmentIndex, int x, int y)
	{
		DE_ASSERT(AttachmentIndex < ColorAttachments.size(), "Attachment Index is out of range");

		Bind();
		glReadBuffer(GL_COLOR_ATTACHMENT0 + AttachmentIndex);

		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

		Unbind();

		return pixelData;
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t AttachmentIndex, int value)
	{
		DE_ASSERT(AttachmentIndex < ColorAttachments.size(), "Attachment Index is out of range");

		auto& spec = ColorAttachmentSpecifications[AttachmentIndex];

		Bind();
		glClearTexImage(
			ColorAttachments[AttachmentIndex],
			0,
			Utils::GetGLFormat(spec.TextureFormat),
			GL_INT,
			&value
		);
		Unbind();
	}
}