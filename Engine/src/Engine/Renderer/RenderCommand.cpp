#include "depch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Engine
{
	RendererAPI* RenderCommand::API_Instance = new OpenGLRendererAPI();

	void RenderCommand::SetClearColor(const glm::vec4& color)
	{
		API_Instance->SetClearColor(color);
	}

	void RenderCommand::Clear()
	{
		API_Instance->Clear();
	}

	void RenderCommand::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		API_Instance->DrawIndexed(vertexArray);
	}
}