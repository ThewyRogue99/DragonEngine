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

		static void DrawIndexed(const Ref<VertexArray>& vertexArray);

	private:
		static RendererAPI* API_Instance;
	};
}
