#include "depch.h"
#include "Renderer2D.h"

#include "ShapeBuffer.h"

#include "RenderCommand.h"
#include "Engine/Renderer/UniformBuffer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		// Editor-only
		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		// Editor-only
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

		Ref<Texture2D> WhiteTexture;

		float LineWidth = 1.f;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		glm::vec4 QuadVertexPositions[4] = { };

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection = glm::mat4(1.f);
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

		Ref<ShapeBuffer<QuadVertex>> QuadShapeBuffer = nullptr;
		Ref<ShapeBuffer<CircleVertex>> CircleShapeBuffer = nullptr;
		Ref<ShapeBuffer<LineVertex>> LineShapeBuffer = nullptr;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		DE_PROFILE_FUNCTION();

		s_Data.QuadShapeBuffer = CreateRef<ShapeBuffer<QuadVertex>>(s_Data.MaxQuads, 4);
		s_Data.QuadShapeBuffer->CreateBuffer({
			{ ShaderDataType::Float3, TEXT("a_Position")     },
			{ ShaderDataType::Float4, TEXT("a_Color")        },
			{ ShaderDataType::Float2, TEXT("a_TexCoord")     },
			{ ShaderDataType::Float,  TEXT("a_TexIndex")     },
			{ ShaderDataType::Float,  TEXT("a_TilingFactor") },
			{ ShaderDataType::Int,    TEXT("a_EntityID")     }
		});

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadShapeBuffer->GetVertexArray()->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// Circles

		s_Data.CircleShapeBuffer = CreateRef<ShapeBuffer<CircleVertex>>(s_Data.MaxQuads, 4);
		s_Data.CircleShapeBuffer->CreateBuffer({
			{ ShaderDataType::Float3, TEXT("a_WorldPosition") },
			{ ShaderDataType::Float3, TEXT("a_LocalPosition") },
			{ ShaderDataType::Float4, TEXT("a_Color")         },
			{ ShaderDataType::Float,  TEXT("a_Thickness")     },
			{ ShaderDataType::Float,  TEXT("a_Fade")          },
			{ ShaderDataType::Int,    TEXT("a_EntityID")      }
		});
		s_Data.CircleShapeBuffer->GetVertexArray()->SetIndexBuffer(quadIB);

		// Lines

		s_Data.LineShapeBuffer = CreateRef<ShapeBuffer<LineVertex>>(s_Data.MaxQuads, 2);
		s_Data.LineShapeBuffer->CreateBuffer({
			{ ShaderDataType::Float3, TEXT("a_Position") },
			{ ShaderDataType::Float4, TEXT("a_Color")    },
			{ ShaderDataType::Int,    TEXT("a_EntityID") }
		});

		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextureSlots] = { };
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		SetFlushCallbacks();

		s_Data.QuadShapeBuffer->SetShader(Shader::Create(
			TEXT("QuadShader"),
			"../Engine/src/Engine/Renderer/Shaders/QuadVertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/QuadFragment.glsl",
			true
		));

		s_Data.CircleShapeBuffer->SetShader(Shader::Create(
			TEXT("CircleShader"),
			"../Engine/src/Engine/Renderer/Shaders/CircleVertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/CircleFragment.glsl",
			true
		));

		s_Data.LineShapeBuffer->SetShader(Shader::Create(
			TEXT("LineShader"),
			"../Engine/src/Engine/Renderer/Shaders/LineVertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/LineFragment.glsl",
			true
		));

		// Set first texture slot to 0
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		DE_PROFILE_FUNCTION();

		s_Data.QuadShapeBuffer = nullptr;
		s_Data.CircleShapeBuffer = nullptr;
		s_Data.LineShapeBuffer = nullptr;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		DE_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		DE_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer2D::StartBatch()
	{
		s_Data.QuadShapeBuffer->Begin();

		s_Data.CircleShapeBuffer->Begin();

		s_Data.LineShapeBuffer->Begin();

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		s_Data.QuadShapeBuffer->End();

		s_Data.CircleShapeBuffer->End();

		s_Data.LineShapeBuffer->End();
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		DE_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DE_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		DE_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		s_Data.QuadShapeBuffer->DrawShape(
		[transform, color, textureIndex, tilingFactor, textureCoords, entityID](size_t idx)
		{
			QuadVertex val = {
				transform * s_Data.QuadVertexPositions[idx],
				color,
				textureCoords[idx],
				textureIndex,
				tilingFactor,
				entityID
			};

			return val;
		});

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		DE_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots.at(s_Data.TextureSlotIndex) = texture;
			s_Data.TextureSlotIndex++;
		}

		s_Data.QuadShapeBuffer->DrawShape(
		[transform, tintColor, textureIndex, tilingFactor, textureCoords, entityID](size_t idx)
		{
			QuadVertex val = {
				transform * s_Data.QuadVertexPositions[idx],
				tintColor,
				textureCoords[idx],
				textureIndex,
				tilingFactor,
				entityID
			};

			return val;
		});

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DE_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DE_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
	{
		DE_PROFILE_FUNCTION();

		s_Data.CircleShapeBuffer->DrawShape(
		[transform, color, thickness, fade, entityID](size_t idx)
		{
			CircleVertex val = {
				transform * s_Data.QuadVertexPositions[idx],
				s_Data.QuadVertexPositions[idx] * 2.0f,
				color,
				thickness,
				fade,
				entityID
			};

			return val;
		});

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color)
	{
		s_Data.LineShapeBuffer->DrawVertex({ p0, color });

		s_Data.LineShapeBuffer->DrawVertex({ p1, color });
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color);
		DrawLine(p1, p2, color);
		DrawLine(p2, p3, color);
		DrawLine(p3, p0, color);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_Data.QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color);
		DrawLine(lineVertices[1], lineVertices[2], color);
		DrawLine(lineVertices[2], lineVertices[3], color);
		DrawLine(lineVertices[3], lineVertices[0], color);
	}

	void Renderer2D::DrawQuadSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		if (src.Texture)
			DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
		else
			DrawQuad(transform, src.Color, entityID);
	}

	void Renderer2D::SetLineWidth(float Width)
	{
		s_Data.LineWidth = Width;
	}

	float Renderer2D::GetLineWidth()
	{
		return s_Data.LineWidth;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

	void Renderer2D::SetFlushCallbacks()
	{
		s_Data.QuadShapeBuffer->OnFlush([]()
		{
			// Bind textures
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);

			size_t IndexCount = s_Data.QuadShapeBuffer->GetShapeCount() * 6;

			RenderCommand::DrawIndexed(
				s_Data.QuadShapeBuffer->GetVertexArray(),
				IndexCount
			);

			s_Data.Stats.DrawCalls++;
		});

		s_Data.CircleShapeBuffer->OnFlush([]()
		{
			size_t IndexCount = s_Data.CircleShapeBuffer->GetShapeCount() * 6;

			RenderCommand::DrawIndexed(
				s_Data.CircleShapeBuffer->GetVertexArray(),
				IndexCount
			);

			s_Data.Stats.DrawCalls++;
		});

		s_Data.LineShapeBuffer->OnFlush([]()
		{
			size_t IndexCount = s_Data.LineShapeBuffer->GetShapeCount() * 2;

			RenderCommand::SetLineWidth(s_Data.LineWidth);
			RenderCommand::DrawIndexed(
				s_Data.LineShapeBuffer->GetVertexArray(),
				IndexCount
			);

			s_Data.Stats.DrawCalls++;
		});
	}

}