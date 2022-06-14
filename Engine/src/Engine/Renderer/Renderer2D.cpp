#include "depch.h"
#include "Renderer2D.h"
#include "Shader.h"
#include "Engine/Renderer/UniformBuffer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		// Editor-Only
		int EntityID;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RendererCaps

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;

		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;

		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		glm::vec3 QuadVertexPositions[4] = {
			{ -0.5f, -0.5f,  0.0f },
			{  0.5f, -0.5f,  0.0f },
			{  0.5f,  0.5f,  0.0f },
			{ -0.5f,  0.5f,  0.0f }
		};

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		} CameraBuffer;

		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DData Data;

	void Renderer2D::Init()
	{
		DE_PROFILE_FUNCTION();

		Data.QuadVertexArray = Engine::VertexArray::Create();

		Data.QuadVertexBuffer = Engine::VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(QuadVertex));

		Data.QuadVertexBuffer->SetLayout({
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float4, "a_Color" },
			{ Engine::ShaderDataType::Float2, "a_TexCoord" },
			{ Engine::ShaderDataType::Float, "a_TexIndex" },
			{ Engine::ShaderDataType::Float, "a_TilingFactor" },
			{ Engine::ShaderDataType::Int, "a_EntityID" }
		});

		Data.QuadVertexArray->AddVertexBuffer(Data.QuadVertexBuffer);

		Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MaxVertices];

		uint32_t* QuadIndices = new uint32_t[Renderer2DData::MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < Renderer2DData::MaxIndices; i += 6)
		{
			QuadIndices[i + 0] = offset + 0;
			QuadIndices[i + 1] = offset + 1;
			QuadIndices[i + 2] = offset + 2;

			QuadIndices[i + 3] = offset + 2;
			QuadIndices[i + 4] = offset + 3;
			QuadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		auto IndexBuffer = Engine::IndexBuffer::Create(QuadIndices, Renderer2DData::MaxIndices);

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

		Data.TextureSlots[0] = Data.WhiteTexture;

		Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		DE_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4 transform)
	{
		DE_PROFILE_FUNCTION();

		Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		Data.CameraUniformBuffer->SetData(&Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		Data.QuadIndexCount = 0;
		Data.QuadVertexBufferPtr = Data.QuadVertexBufferBase;

		Data.TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		DE_PROFILE_FUNCTION();

		Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		Data.CameraUniformBuffer->SetData(&Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		Data.QuadIndexCount = 0;
		Data.QuadVertexBufferPtr = Data.QuadVertexBufferBase;

		Data.TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		DE_PROFILE_FUNCTION();

		glm::mat4 ViewProjection = camera.GetViewProjectionMatrix();

		Data.TextureShader->SetMat4("ViewProjection", ViewProjection);

		Data.QuadIndexCount = 0;
		Data.QuadVertexBufferPtr = Data.QuadVertexBufferBase;

		Data.TextureSlotIndex = 1;
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
		for (uint32_t i = 0; i < Data.TextureSlotIndex; i++)
			Data.TextureSlots[i]->Bind(i);

		Data.TextureShader->Bind();
		RenderCommand::DrawIndexed(Data.QuadVertexArray, Data.QuadIndexCount);

		Data.Stats.DrawCalls++;
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		Data.QuadIndexCount = 0;
		Data.QuadVertexBufferPtr = Data.QuadVertexBufferBase;

		Data.TextureSlotIndex = 1;
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		DE_PROFILE_FUNCTION();

		glm::mat4 transform = glm::scale(
			glm::translate(
				glm::mat4(1.f),
				position
			),
			{ size.x, size.y, 0.f }
		);

		DrawQuadWithTransform(transform, nullptr, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D> texture)
	{
		DE_PROFILE_FUNCTION();

		glm::mat4 transform = glm::scale(
			glm::translate(
				glm::mat4(1.f),
				position
			),
			{ size.x, size.y, 0.f }
		);

		DrawQuadWithTransform(transform, texture, glm::vec4(1.f));
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		DE_PROFILE_FUNCTION();

		DrawQuadWithTransform(transform, nullptr, color);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D> texture)
	{
		DE_PROFILE_FUNCTION();

		DrawQuadWithTransform(transform, texture, glm::vec4(1.f));
	}

	void Renderer2D::DrawQuadSprite(const glm::mat4& transform, SpriteRendererComponent& src, int EntityID)
	{
		DE_PROFILE_FUNCTION();

		DrawQuadWithTransform(transform, nullptr, src.Color, EntityID);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, float Rotation, const glm::vec2& size, const glm::vec4& color)
	{
		DE_PROFILE_FUNCTION();

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

		DrawQuadWithTransform(transform, nullptr, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, float Rotation, const glm::vec2& size, const Ref<Texture2D> texture)
	{
		DE_PROFILE_FUNCTION();

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

		DrawQuadWithTransform(transform, texture, glm::vec4(1.f));
	}

	void Renderer2D::ResetStats()
	{
		memset(&(Data.Stats), 0, sizeof(Data.Stats));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return Data.Stats;
	}

	void Renderer2D::DrawQuadWithTransform(const glm::mat4& transform, Ref<Texture2D> texture, const glm::vec4& color, int EntityID)
	{
		if (Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();


		bool bIsUsingTexture = texture ? true : false;
		
		glm::vec4 VertexColor = bIsUsingTexture ? glm::vec4(1.f) : color;
		float VertexTilingFactor = 1.f;

		uint32_t textureIndex = 0;

		if (bIsUsingTexture)
		{
			for (uint32_t i = 1; i < Data.TextureSlotIndex; i++)
			{
				if (*Data.TextureSlots[i].get() == *texture.get())
				{
					textureIndex = i;
					break;
				}
			}

			if (textureIndex == 0)
			{
				textureIndex = Data.TextureSlotIndex;
				Data.TextureSlots[textureIndex] = texture;

				Data.TextureSlotIndex++;
			}
		}

		Data.QuadVertexBufferPtr->Position = transform * glm::vec4(Data.QuadVertexPositions[0], 1.f);
		Data.QuadVertexBufferPtr->Color = VertexColor;
		Data.QuadVertexBufferPtr->TexCoord = { 0.f, 0.f };
		Data.QuadVertexBufferPtr->TexIndex = (float)textureIndex;
		Data.QuadVertexBufferPtr->TilingFactor = VertexTilingFactor;
		Data.QuadVertexBufferPtr->EntityID = EntityID;
		Data.QuadVertexBufferPtr++;

		Data.QuadVertexBufferPtr->Position = transform * glm::vec4(Data.QuadVertexPositions[1], 1.f);
		Data.QuadVertexBufferPtr->Color = VertexColor;
		Data.QuadVertexBufferPtr->TexCoord = { 1.f, 0.f };
		Data.QuadVertexBufferPtr->TexIndex = (float)textureIndex;
		Data.QuadVertexBufferPtr->TilingFactor = VertexTilingFactor;
		Data.QuadVertexBufferPtr->EntityID = EntityID;
		Data.QuadVertexBufferPtr++;

		Data.QuadVertexBufferPtr->Position = transform * glm::vec4(Data.QuadVertexPositions[2], 1.f);
		Data.QuadVertexBufferPtr->Color = VertexColor;
		Data.QuadVertexBufferPtr->TexCoord = { 1.f, 1.f };
		Data.QuadVertexBufferPtr->TexIndex = (float)textureIndex;
		Data.QuadVertexBufferPtr->TilingFactor = VertexTilingFactor;
		Data.QuadVertexBufferPtr->EntityID = EntityID;
		Data.QuadVertexBufferPtr++;

		Data.QuadVertexBufferPtr->Position = transform * glm::vec4(Data.QuadVertexPositions[3], 1.f);
		Data.QuadVertexBufferPtr->Color = VertexColor;
		Data.QuadVertexBufferPtr->TexCoord = { 0.f, 1.f };
		Data.QuadVertexBufferPtr->TexIndex = (float)textureIndex;
		Data.QuadVertexBufferPtr->TilingFactor = VertexTilingFactor;
		Data.QuadVertexBufferPtr->EntityID = EntityID;
		Data.QuadVertexBufferPtr++;

		Data.QuadIndexCount += 6;

		Data.Stats.QuadCount++;
	}
}