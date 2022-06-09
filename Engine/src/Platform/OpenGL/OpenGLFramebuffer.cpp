#include "depch.h"
#include "OpenGLFramebuffer.h"

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

		static void AttachColorTexture(uint32_t id, int samples, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;

			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

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
		glDeleteTextures(ColorAttachments.size(), ColorAttachments.data());
		glDeleteTextures(1, &DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (RendererID)
		{
			glDeleteFramebuffers(1, &RendererID);
			glDeleteTextures(ColorAttachments.size(), ColorAttachments.data());
			glDeleteTextures(1, &DepthAttachment);

			ColorAttachments.clear();
			DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, RendererID);

		// Attachments

		bool multisample = BufferSpecification.Samples > 1;

		if (ColorAttachmentSpecifications.size() > 0)
		{
			ColorAttachments.resize(ColorAttachmentSpecifications.size());
			Utils::CreateTextures(multisample, ColorAttachments.data(), ColorAttachments.size());

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
							BufferSpecification.Width,
							BufferSpecification.Height,
							i
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
			DE_CORE_ASSERT(ColorAttachments.size() <= 4, "Engine only supports 4 color attachments!");

			GLenum buffers[4] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2,
				GL_COLOR_ATTACHMENT3
			};

			glDrawBuffers(ColorAttachments.size(), buffers);
		}
		else if (ColorAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}

		DE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, RendererID);
		glViewport(0, 0, BufferSpecification.Width, BufferSpecification.Height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t Width, uint32_t Height)
	{
		BufferSpecification.Width = Width;
		BufferSpecification.Height = Height;

		Invalidate();
	}
}