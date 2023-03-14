#pragma once

#include "Engine/Core/Core.h"

#include "Renderer.h"

#include <glm/glm.hpp>

namespace Engine
{
	class Framebuffer;
	class Texture2D;

	struct QuadDrawProperties
	{
		glm::mat4 Transform = glm::mat4(1.f);
		glm::vec4 Color = glm::vec4(1.f);

		Ref<Texture2D> TextureRef = nullptr;
		float TilingFactor = 1.f;
	};

	struct CircleDrawProperties
	{
		glm::mat4 Transform = glm::mat4(1.f);
		glm::vec4 Color = glm::vec4(1.f);
		float Thickness = 1.f;
		float Fade = 0.005f;
	};

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame(Ref<Framebuffer> FramebufferRef);
		static void EndFrame();

		static void DrawQuad(
			const QuadDrawProperties& Props,
			int entityID = -1
		);

		static void DrawCircle(
			const CircleDrawProperties& Props,
			int entityID = -1
		);

		static void DrawLine(
			const glm::vec3& p0,
			glm::vec3& p1,
			const glm::vec4& color
		);

		static void DrawRect(
			const glm::vec3& position,
			const glm::vec2& size,
			const glm::vec4& color
		);

		static void SetLineWidth(float Width);
		static float GetLineWidth();

		static void ResetStats();

	private:
		static void StartBatch();
		static void NextBatch();
		static void Flush();

		static void FlushQuads();
		static void FlushCircles();
		static void FlushLines();

	public:
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static const Statistics& GetStats();

	private:
		struct RenderProperties
		{
			float TilingFactor = 1.f;
		};
		
		static void SetFlushCallbacks();
	};
}
