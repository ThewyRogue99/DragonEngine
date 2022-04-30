#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Engine
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None, OpenGL,
		};

	public:
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;

		inline static API GetAPI() { return API_Instance; }

	private:
		static API API_Instance;
	};
}
