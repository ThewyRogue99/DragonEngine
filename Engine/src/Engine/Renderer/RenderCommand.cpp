#include "depch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Engine
{
	RendererAPI* RenderCommand::API_Instance = new OpenGLRendererAPI();

	void RenderCommand::Init()
	{
		API_Instance->Init();
	}

	void RenderCommand::SetClearColor(const glm::vec4& color)
	{
		API_Instance->SetClearColor(color);
	}

	void RenderCommand::Clear()
	{
		API_Instance->Clear();
	}

	void RenderCommand::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t IndexCount)
	{
		API_Instance->DrawIndexed(vertexArray, IndexCount);
	}

	void RenderCommand::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t VertexCount)
	{
		API_Instance->DrawLines(vertexArray, VertexCount);
	}

	void RenderCommand::SetLineWidth(float Width)
	{
		API_Instance->SetLineWidth(Width);
	}

	void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		API_Instance->SetViewport(x, y, width, height);
	}
}