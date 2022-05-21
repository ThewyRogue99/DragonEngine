#pragma once

namespace Engine
{
	class OrthographicCamera;

	class Renderer2D
	{
	public:
		struct RenderProperties;

		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		static void DrawQuad(
			const glm::vec3& position,
			const glm::vec2& size,
			const glm::vec4& color,
			RenderProperties& properties = RenderProperties()
		);

		static void DrawQuad(
			const glm::vec3& position,
			const glm::vec2& size,
			const Ref<Texture2D> texture,
			RenderProperties& properties = RenderProperties()
		);

		static void DrawQuad(
			const glm::mat4& transform,
			const glm::vec4& color,
			RenderProperties& properties = RenderProperties()
		);

		static void DrawQuad(
			const glm::mat4& transform,
			const Ref<Texture2D> texture,
			RenderProperties& properties = RenderProperties()
		);

		static void DrawRotatedQuad(
			const glm::vec3& position,
			float Rotation,
			const glm::vec2& size,
			const glm::vec4& color,
			RenderProperties& properties = RenderProperties()
		);

		static void DrawRotatedQuad(
			const glm::vec3& position,
			float Rotation,
			const glm::vec2& size,
			const Ref<Texture2D> texture,
			RenderProperties& properties = RenderProperties()
		);

		static void ResetStats();

	public:
		struct RenderProperties
		{
			float TilingFactor = 1.f;
		};

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
			RenderProperties& properties = RenderProperties()
		);

		static void FlushAndReset();
	};
}
