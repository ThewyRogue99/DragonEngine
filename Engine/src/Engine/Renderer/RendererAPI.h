#pragma once

#include "Engine/Core/Core.h"
#include "VertexArray.h"

#include <glm/glm.hpp>

namespace Engine
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None, OpenGL
		};

	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t IndexCount = 0) = 0;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t VertexCount = 0) = 0;

		virtual void SetLineWidth(float width) = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		inline static API GetAPI() { return m_API; }

		static RendererAPI* Create(API api);

	private:
		static API m_API;
	};
}
