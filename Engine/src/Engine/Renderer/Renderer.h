#pragma once

#include "RendererAPI.h"

#include "Shader.h"
#include "VertexArray.h"
#include "Framebuffer.h"

namespace Engine
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame(Ref<Framebuffer> FramebufferRef);
		static void EndFrame();

		static void SetLineWidth(float Width);

		static void DrawIndexed(Ref<Shader> ShaderRef, Ref<VertexArray> VertexArrayRef, uint32_t IndexCount = 0);
		static void DrawLine(Ref<Shader> ShaderRef, Ref<VertexArray> VertexArrayRef, uint32_t VertexCount = 0);

		static RendererAPI* GetRendererAPI();
	};
}