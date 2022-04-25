#pragma once

#include <glm/glm.hpp>

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

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;

		inline static API GetAPI() { return API_Instance; }

	private:
		static API API_Instance;
	};
}
