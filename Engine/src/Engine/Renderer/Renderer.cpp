#include "depch.h"
#include "Renderer.h"

namespace Engine
{
	Renderer::SceneData Renderer::sceneData = Renderer::SceneData();

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		sceneData.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const glm::mat4& transform)
	{
		shader->Bind();
		vertexArray->Bind();

		shader->SetUniformMat4("ViewProjection", sceneData.ViewProjectionMatrix);
		shader->SetUniformMat4("Transform", transform);

		RenderCommand::DrawIndexed(vertexArray);
	}

}