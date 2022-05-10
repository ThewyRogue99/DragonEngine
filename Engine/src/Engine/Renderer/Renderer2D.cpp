#include "depch.h"
#include "Renderer2D.h"

#include "Shader.h"

namespace Engine
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
	};

	struct Renderer2DData
	{
		const uint32_t MaxQuads = 10000;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices = MaxQuads * 6;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;

		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;

		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;
	};

	static Renderer2DData Data;

	void Renderer2D::Init()
	{
		DE_PROFILE_FUNCTION();

		Data.QuadVertexArray = Engine::VertexArray::Create();

		Data.QuadVertexBuffer = Engine::VertexBuffer::Create(Data.MaxVertices * sizeof(QuadVertex));

		Data.QuadVertexBuffer->SetLayout({
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float4, "a_Color" },
			{ Engine::ShaderDataType::Float2, "a_TexCoord" }
		});

		Data.QuadVertexArray->AddVertexBuffer(Data.QuadVertexBuffer);

		Data.QuadVertexBufferBase = new QuadVertex[Data.MaxVertices];

		uint32_t* QuadIndices = new uint32_t[Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < Data.MaxIndices; i += 6)
		{
			QuadIndices[i + 0] = offset + 0;
			QuadIndices[i + 1] = offset + 1;
			QuadIndices[i + 2] = offset + 2;

			QuadIndices[i + 3] = offset + 2;
			QuadIndices[i + 4] = offset + 3;
			QuadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		auto IndexBuffer = Engine::IndexBuffer::Create(QuadIndices, Data.MaxIndices);

		Data.QuadVertexArray->SetIndexBuffer(IndexBuffer);

		delete[] QuadIndices;

		Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		Data.WhiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));

		Data.TextureShader = Engine::Shader::Create(
			"TextureShader",
			"../Engine/src/Engine/Renderer/Shaders/TextureVertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/TextureFragment.glsl",
			true
		);

		Data.TextureShader->Bind();
		Data.TextureShader->SetInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		DE_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		DE_PROFILE_FUNCTION();

		glm::mat4 ViewProjection = camera.GetViewProjectionMatrix();

		Data.TextureShader->SetMat4("ViewProjection", ViewProjection);

		Data.QuadIndexCount = 0;
		Data.QuadVertexBufferPtr = Data.QuadVertexBufferBase;
	}

	void Renderer2D::EndScene()
	{
		DE_PROFILE_FUNCTION();

		uint32_t DataSize = (uint32_t)((uint8_t*)Data.QuadVertexBufferPtr - (uint8_t*)Data.QuadVertexBufferBase);
		Data.QuadVertexBuffer->SetData(Data.QuadVertexBufferBase, DataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		RenderCommand::DrawIndexed(Data.QuadVertexArray, Data.QuadIndexCount);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		DE_PROFILE_FUNCTION();

		Data.QuadVertexBufferPtr->Position = position;
		Data.QuadVertexBufferPtr->Color = color;
		Data.QuadVertexBufferPtr->TexCoord = { 0.f, 0.f };
		Data.QuadVertexBufferPtr++;

		Data.QuadVertexBufferPtr->Position = { position.x + size.x, position.y, 0.f };
		Data.QuadVertexBufferPtr->Color = color;
		Data.QuadVertexBufferPtr->TexCoord = { 1.f, 0.f };
		Data.QuadVertexBufferPtr++;

		Data.QuadVertexBufferPtr->Position = { position.x + size.x, position.y + size.y, 0.f };
		Data.QuadVertexBufferPtr->Color = color;
		Data.QuadVertexBufferPtr->TexCoord = { 1.f, 1.f };
		Data.QuadVertexBufferPtr++;

		Data.QuadVertexBufferPtr->Position = { position.x, position.y + size.y, 0.f };
		Data.QuadVertexBufferPtr->Color = color;
		Data.QuadVertexBufferPtr->TexCoord = { 0.f, 1.f };
		Data.QuadVertexBufferPtr++;

		Data.QuadIndexCount += 6;
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D> texture)
	{
		glm::mat4 transform = glm::scale(
			glm::translate(
				glm::mat4(1.f),
				position
			),
			{ size.x, size.y, 0.f }
		);

		Data.TextureShader->SetFloat4("Color", glm::vec4(1.f));
		Data.TextureShader->SetMat4("Transform", transform);

		texture->Bind();

		RenderCommand::DrawIndexed(Data.QuadVertexArray);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, float Rotation, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::scale(
			glm::rotate(
				glm::translate(
					glm::mat4(1.f),
					position
				),
				glm::radians(Rotation), { 0.f, 0.f, 1.f }
			),
			{ size.x, size.y, 0.f }
		);

		Data.TextureShader->SetFloat4("Color", color);
		Data.TextureShader->SetMat4("Transform", transform);

		Data.WhiteTexture->Bind();

		RenderCommand::DrawIndexed(Data.QuadVertexArray);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, float Rotation, const glm::vec2& size, const Ref<Texture2D> texture)
	{
		glm::mat4 transform = glm::scale(
			glm::rotate(
				glm::translate(
					glm::mat4(1.f),
					position
				),
				glm::radians(Rotation), { 0.f, 0.f, 1.f }
			),
			{ size.x, size.y, 0.f }
		);

		Data.TextureShader->SetFloat4("Color", glm::vec4(1.f));
		Data.TextureShader->SetMat4("Transform", transform);

		texture->Bind();

		RenderCommand::DrawIndexed(Data.QuadVertexArray);
	}
}