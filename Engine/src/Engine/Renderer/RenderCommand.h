#pragma once

#include "RendererAPI.h"

namespace Engine
{
	class RenderCommand
	{
	public:
		static void Init();

		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t IndexCount = 0);

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

	private:
		static RendererAPI* API_Instance;
	};
}
