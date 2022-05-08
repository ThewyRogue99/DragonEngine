#include "depch.h"
#include "Renderer.h"

#include "Renderer2D.h"

namespace Engine
{
	Renderer::SceneData Renderer::sceneData = Renderer::SceneData();

	void Renderer::Init()
	{
		RenderCommand::Init();

		Renderer2D::Init();
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		sceneData.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
		
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		vertexArray->Bind();

		shader->SetMat4("ViewProjection", sceneData.ViewProjectionMatrix);
		shader->SetMat4("Transform", transform);

		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}
}