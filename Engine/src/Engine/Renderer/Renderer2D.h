#pragma once

#include "Engine/Core/Core.h"

#include "Camera.h"
#include "Engine/Scene/Components.h"

#include "Texture.h"

#include <glm/glm.hpp>

namespace Engine
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void EndScene();

		static void StartBatch();
		static void NextBatch();
		static void Flush();

		static void DrawQuad(
			const glm::vec3& position,
			const glm::vec2& size,
			const glm::vec4& color
		);

		static void DrawQuad(
			const glm::vec3& position,
			const glm::vec2& size,
			const Ref<Texture2D>& texture,
			float tilingFactor,
			const glm::vec4& tintColor
		);

		static void DrawQuad(
			const glm::mat4& transform,
			const glm::vec4& color,
			int entityID = -1
		);

		static void DrawQuad(
			const glm::mat4& transform,
			const Ref<Texture2D>& texture,
			float tilingFactor,
			const glm::vec4& tintColor,
			int entityID = -1
		);

		static void DrawQuadSprite(
			const glm::mat4& transform,
			SpriteRendererComponent& src,
			int EntityID = -1
		);

		static void DrawRotatedQuad(
			const glm::vec3& position,
			const glm::vec2& size,
			float rotation,
			const glm::vec4& color
		);

		static void DrawRotatedQuad(
			const glm::vec3& position,
			const glm::vec2& size,
			float rotation,
			const Ref<Texture2D>& texture,
			float tilingFactor,
			const glm::vec4& tintColor
		);

		static void DrawCircle(
			const glm::mat4& transform,
			const glm::vec4& color,
			float thickness = 1.f,
			float fade = 0.005f,
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

		static void DrawRect(
			const glm::mat4& transform,
			const glm::vec4& color
		);

		static void SetLineWidth(float Width);
		static float GetLineWidth();

		static void ResetStats();

	public:
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static Statistics GetStats();

	private:
		struct RenderProperties
		{
			float TilingFactor = 1.f;
		};
		
		static void SetFlushCallbacks();
	};
}
