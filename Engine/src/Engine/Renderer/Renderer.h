#pragma once

#include "RenderCommand.h"
#include "OrthographicCamera.h"
#include "Shader.h"

namespace Engine
{
	class Renderer
	{
	public:
		static void Init();

		static void BeginScene(OrthographicCamera& camera);

		static void EndScene();

		static void Submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform = glm::mat4(1.f));

		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix = glm::mat4(1.f);
		};

		static SceneData sceneData;
	};
}