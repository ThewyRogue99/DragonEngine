#pragma once

#include "Engine/Core/Core.h"

namespace Engine
{
	struct FramebufferProps
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t Width, uint32_t Height) = 0;

		virtual uint32_t GetColorAttachmentRendererID() const = 0;

		virtual const FramebufferProps& GetSpecification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferProps& props);
	};
}
