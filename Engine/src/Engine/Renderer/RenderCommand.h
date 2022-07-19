#pragma once

#include "Engine/Core/Core.h"
#include "RendererAPI.h"

#include "VertexArray.h"

#include <glm/glm.hpp>

namespace Engine
{
	class RenderCommand
	{
	public:
		static void Init();

		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t IndexCount = 0);
		static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t VertexCount = 0);

		static void SetLineWidth(float Width);

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

	private:
		static RendererAPI* API_Instance;
	};
}
