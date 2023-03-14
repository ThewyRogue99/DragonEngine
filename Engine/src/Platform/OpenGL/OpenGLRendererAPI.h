#pragma once

#include "Engine/Renderer/RendererAPI.h"

namespace Engine
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear(Ref<Framebuffer> FramebufferRef) override;

		virtual void DrawIndexed(const IndexedDrawProperties& Props) override;
		virtual void DrawLine(const LineDrawProperties& Props) override;

		virtual void SetLineWidth(float Width) override;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	};
}
