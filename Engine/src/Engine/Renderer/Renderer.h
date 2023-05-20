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
		ENGINE_API static void SetAPI(RendererAPI::API API);
		ENGINE_API static RendererAPI::API GetAPI();

		ENGINE_API static void Init();
		ENGINE_API static void Shutdown();

		ENGINE_API static void BeginFrame(Ref<Framebuffer> FramebufferRef);
		ENGINE_API static void EndFrame();

		ENGINE_API static void SetLineWidth(float Width);

		ENGINE_API static void DrawIndexed(Ref<Shader> ShaderRef, Ref<VertexArray> VertexArrayRef, uint32_t IndexCount = 0);
		ENGINE_API static void DrawLine(Ref<Shader> ShaderRef, Ref<VertexArray> VertexArrayRef, uint32_t VertexCount = 0);

		ENGINE_API static const Ref<RendererAPI>& GetRendererAPI();
	};
}