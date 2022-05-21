#pragma once

#include "Engine/Renderer/Framebuffer.h"

namespace Engine
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferProps& props);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t Width, uint32_t Height) override;

		inline virtual const FramebufferProps& GetSpecification() const override { return Props; }

		inline virtual uint32_t GetColorAttachmentRendererID() const override { return ColorAttachment; }

	private:
		uint32_t RendererID = 0;
		uint32_t ColorAttachment = 0, DepthAttachment = 0;

		FramebufferProps Props;
	};
}
