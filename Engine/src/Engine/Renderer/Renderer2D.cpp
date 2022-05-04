#include "depch.h"
#include "Renderer2D.h"

#include "Shader.h"

namespace Engine
{
	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> FlatColorShader;
		Ref<Shader> TextureShader;
	};

	static Renderer2DStorage* Data;

	void Renderer2D::Init()
	{
		Data = new Renderer2DStorage();

		Data->QuadVertexArray = Engine::VertexArray::Create();

		float SquareVertices[] = {
			-0.5f, -0.5f,  0.0f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.0f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.0f,  0.0f, 1.0f
		};

		auto VertexBuffer = Engine::VertexBuffer::Create(SquareVertices, sizeof(SquareVertices));

		VertexBuffer->SetLayout({
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float2, "a_TexCoord" },
		});

		Data->QuadVertexArray->AddVertexBuffer(VertexBuffer);

		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
		auto IndexBuffer = Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

		Data->QuadVertexArray->SetIndexBuffer(IndexBuffer);

		Data->FlatColorShader = Engine::Shader::Create(
			"FlatColorShader",
			"../Engine/src/Engine/Renderer/Shaders/MaterialVertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/MaterialFragment.glsl",
			true
		);

		Data->TextureShader = Engine::Shader::Create(
			"TextureShader",
			"../Engine/src/Engine/Renderer/Shaders/TextureVertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/TextureFragment.glsl",
			true
		);

		Data->TextureShader->Bind();
		Data->TextureShader->SetInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		delete Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		glm::mat4 ViewProjection = camera.GetViewProjectionMatrix();

		Data->FlatColorShader->SetMat4("ViewProjection", ViewProjection);

		Data->TextureShader->SetMat4("ViewProjection", ViewProjection);
	}

	void Renderer2D::EndScene()
	{

	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform =
			glm::translate(glm::mat4(1.f), { position.x, position.y, 0.f }) *
			glm::scale(glm::mat4(1.f), { size.x, size.y, 0.f });

		Data->FlatColorShader->SetFloat4("Color", color);
		Data->FlatColorShader->SetMat4("Transform", transform);

		RenderCommand::DrawIndexed(Data->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D> texture)
	{
		glm::mat4 transform =
			glm::translate(glm::mat4(1.f), { position.x, position.y, 0.f }) *
			glm::scale(glm::mat4(1.f), { size.x, size.y, 0.f });

		Data->TextureShader->SetMat4("Transform", transform);

		texture->Bind();

		RenderCommand::DrawIndexed(Data->QuadVertexArray);
	}
}