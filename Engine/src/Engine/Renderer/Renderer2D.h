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

		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, RenderProperties properties = RenderProperties());
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D> texture, RenderProperties properties = RenderProperties());

		static void DrawRotatedQuad(const glm::vec3& position, float Rotation, const glm::vec2& size, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, float Rotation, const glm::vec2& size, const Ref<Texture2D> texture);

	public:
		struct RenderProperties
		{
			float TilingFactor = 1.f;
		};
	};
}
