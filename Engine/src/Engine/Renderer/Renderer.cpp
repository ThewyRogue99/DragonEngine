#include "depch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

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

		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4("ViewProjection", sceneData.ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4("Transform", transform);

		RenderCommand::DrawIndexed(vertexArray);
	}

}