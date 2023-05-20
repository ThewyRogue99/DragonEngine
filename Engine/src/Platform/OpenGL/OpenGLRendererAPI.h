#pragma once

#include "Engine/Renderer/RendererAPI.h"

namespace Engine
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		ENGINE_API OpenGLRendererAPI();
		ENGINE_API ~OpenGLRendererAPI();

		ENGINE_API virtual void SetClearColor(const glm::vec4& color) override;
		ENGINE_API virtual void Clear(Ref<Framebuffer> FramebufferRef) override;

		ENGINE_API virtual void DrawIndexed(const IndexedDrawProperties& Props) override;
		ENGINE_API virtual void DrawLine(const LineDrawProperties& Props) override;

		ENGINE_API virtual void SetLineWidth(float Width) override;

		ENGINE_API virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	};
}
