#pragma once

#include "Engine/Core/Core.h"

#include "Shader.h"
#include "VertexArray.h"
#include "RendererAPI.h"

#include <glm/glm.hpp>

namespace Engine
{
	class Renderer
	{
	public:
		static void Init();

		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.f));

		static void OnWindowResize(uint32_t width, uint32_t height);

		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix = glm::mat4(1.f);
		};

		static SceneData sceneData;
	};
}