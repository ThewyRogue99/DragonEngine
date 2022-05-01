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
		virtual void Init() = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;

		inline static API GetAPI() { return m_API; }

	protected:
		inline void SetAPI(API api) { m_API = api; }

	private:
		static API m_API;
	};
}
