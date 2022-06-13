#pragma once

#include "Camera.h"
#include "EditorCamera.h"
#include "Engine/Scene/Components.h"

namespace Engine
{
	class OrthographicCamera;

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4 transform);
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		static void DrawQuad(
			const glm::vec3& position,
			const glm::vec2& size,
			const glm::vec4& color
		);

		static void DrawQuad(
			const glm::vec3& position,
			const glm::vec2& size,
			const Ref<Texture2D> texture
		);

		static void DrawQuad(
			const glm::mat4& transform,
			const glm::vec4& color
		);

		static void DrawQuad(
			const glm::mat4& transform,
			const Ref<Texture2D> texture
		);

		static void DrawQuadSprite(
			const glm::mat4& transform,
			SpriteRendererComponent& src,
			int EntityID = -1
		);

		static void DrawRotatedQuad(
			const glm::vec3& position,
			float Rotation,
			const glm::vec2& size,
			const glm::vec4& color
		);

		static void DrawRotatedQuad(
			const glm::vec3& position,
			float Rotation,
			const glm::vec2& size,
			const Ref<Texture2D> texture
		);

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
		static void DrawQuadWithTransform(
			const glm::mat4& transform,
			Ref<Texture2D> texture = nullptr,
			const glm::vec4& color = glm::vec4(1.f),
			int EntityID = -1
		);

		static void FlushAndReset();
	};
}
