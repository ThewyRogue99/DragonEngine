#pragma once

#include "Engine/Core/Core.h"

#include <glm/glm.hpp>

namespace Engine
{
	class Shader;
	class VertexArray;
	class Framebuffer;

	enum class DrawType
	{
		Triangle,
		Line
	};

	struct DrawProperties
	{
		Ref<Shader> ShaderRef = nullptr;
		Ref<VertexArray> VertexArrayRef = nullptr;
		Ref<Framebuffer> FramebufferRef = nullptr;
	};

	struct IndexedDrawProperties : public DrawProperties
	{
		uint32_t IndexCount = 0;
	};

	struct LineDrawProperties : public DrawProperties
	{
		uint32_t VertexCount = 0;
	};

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
		virtual void Clear(Ref<Framebuffer> FramebufferRef) = 0;

		virtual void DrawIndexed(const IndexedDrawProperties& Props) = 0;
		virtual void DrawLine(const LineDrawProperties& Props) = 0;

		virtual void SetLineWidth(float width) = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		inline static API GetAPI() { return m_API; }

		ENGINE_API static RendererAPI* Create(API api);

	private:
		ENGINE_API static API m_API;
	};
}
