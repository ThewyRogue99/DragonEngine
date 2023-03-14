#pragma once

#include "Engine/Renderer/Framebuffer.h"

namespace Engine
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& Specification);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual void Resize(uint32_t Width, uint32_t Height) override;

		virtual int ReadPixel(uint32_t AttachmentIndex, int x, int y) override;

		virtual void ClearAttachment(uint32_t AttachmentIndex, int value) override;

		inline virtual const FramebufferSpecification& GetSpecification() const override { return BufferSpecification; }

		inline virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { return ColorAttachments[index]; }

	private:
		uint32_t RendererID = 0;

		FramebufferSpecification BufferSpecification;

		std::vector<FramebufferTextureSpecification> ColorAttachmentSpecifications;
		FramebufferTextureSpecification DepthAttachmentSpecification;

		std::vector<uint32_t> ColorAttachments;
		uint32_t DepthAttachment = 0;
	};
}
