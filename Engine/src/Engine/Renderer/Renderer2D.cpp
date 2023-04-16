#include "depch.h"
#include "Renderer2D.h"

#include "Engine/Debug/Instrumentor.h"

#include "Framebuffer.h"
#include "Texture.h"
#include "BatchShapeBuffer.h"

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

	static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

	static const glm::vec4 QuadVertexPositions[4] = {
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{ 0.5f, -0.5f, 0.0f, 1.0f },
			{ 0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f }
	};

	static struct FrameData
	{
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture
	} CurrentFrameData;

	static Renderer2D::Statistics Stats;

	static struct Renderer2DData
	{
		Ref<Texture2D> WhiteTexture;

		struct QuadData
		{
			static const uint32_t MaxQuads = 1000;
			static const uint32_t MaxQuadVertices = MaxQuads * 4;
			static const uint32_t MaxQuadIndices = MaxQuads * 6;

			BatchShapeBuffer<QuadVertex> ShapeBuffer = BatchShapeBuffer<QuadVertex>(MaxQuads, 4);

			Ref<Shader> ShaderRef = nullptr;
		} Quad;

		struct CircleData
		{
			static const uint32_t MaxCircles = 1000;
			static const uint32_t MaxCircleVertices = MaxCircles * 4;

			BatchShapeBuffer<CircleVertex> ShapeBuffer = BatchShapeBuffer<CircleVertex>(MaxCircles, 4);

			Ref<Shader> ShaderRef = nullptr;
		} Circle;

		struct LineData
		{
			static const uint32_t MaxLines = 1000;
			static const uint32_t MaxLineVertices = MaxLines * 2;

			BatchShapeBuffer<LineVertex> ShapeBuffer = BatchShapeBuffer<LineVertex>(MaxLines, 2);

			Ref<Shader> ShaderRef = nullptr;

			float LineWidth = 1.f;
		} Line;
	}* RendererData;

	void Renderer2D::Init()
	{
		DE_PROFILE_FUNCTION();

		RendererData = new Renderer2DData();

		RendererData->Quad.ShapeBuffer.CreateBuffer({
			{ ShaderDataType::Float3, TEXT("a_Position")     },
			{ ShaderDataType::Float4, TEXT("a_Color")        },
			{ ShaderDataType::Float2, TEXT("a_TexCoord")     },
			{ ShaderDataType::Float,  TEXT("a_TexIndex")     },
			{ ShaderDataType::Float,  TEXT("a_TilingFactor") },
			{ ShaderDataType::Int,    TEXT("a_EntityID") }
		});

		uint32_t* quadIndices = new uint32_t[RendererData->Quad.MaxQuadIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < RendererData->Quad.MaxQuadIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, RendererData->Quad.MaxQuadIndices);
		RendererData->Quad.ShapeBuffer.GetVertexArray()->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// Circles

		RendererData->Circle.ShapeBuffer.CreateBuffer({
			{ ShaderDataType::Float3, TEXT("a_WorldPosition") },
			{ ShaderDataType::Float3, TEXT("a_LocalPosition") },
			{ ShaderDataType::Float4, TEXT("a_Color")         },
			{ ShaderDataType::Float,  TEXT("a_Thickness")     },
			{ ShaderDataType::Float,  TEXT("a_Fade")          },
			{ ShaderDataType::Int,    TEXT("a_EntityID") }
		});
		RendererData->Circle.ShapeBuffer.GetVertexArray()->SetIndexBuffer(quadIB);

		// Lines

		RendererData->Line.ShapeBuffer.CreateBuffer({
			{ ShaderDataType::Float3, TEXT("a_Position") },
			{ ShaderDataType::Float4, TEXT("a_Color")    },
			{ ShaderDataType::Int,    TEXT("a_EntityID") }
		});

		RendererData->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		RendererData->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		SetFlushCallbacks();

		RendererData->Quad.ShaderRef = Shader::Create(
			TEXT("QuadShader"),
			"../Engine/src/Engine/Renderer/Shaders/QuadVertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/QuadFragment.glsl",
			true
		);

		RendererData->Circle.ShaderRef = Shader::Create(
			TEXT("CircleShader"),
			"../Engine/src/Engine/Renderer/Shaders/CircleVertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/CircleFragment.glsl",
			true
		);

		RendererData->Line.ShaderRef = Shader::Create(
			TEXT("LineShader"),
			"../Engine/src/Engine/Renderer/Shaders/LineVertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/LineFragment.glsl",
			true
		);

		// Set first texture slot to 0
		CurrentFrameData.TextureSlots[0] = RendererData->WhiteTexture;

		DE_INFO(Renderer2D, "Initialized Renderer2D");
	}

	void Renderer2D::Shutdown()
	{
		DE_PROFILE_FUNCTION();

		delete RendererData;

		DE_INFO(Renderer2D, "Shutting down Renderer2D");
	}

	void Renderer2D::BeginFrame(Ref<Framebuffer> FramebufferRef)
	{
		DE_PROFILE_FUNCTION();

		StartBatch();
		Renderer::BeginFrame(FramebufferRef);
	}

	void Renderer2D::EndFrame()
	{
		DE_PROFILE_FUNCTION();

		Flush();
		Renderer::EndFrame();
	}

	void Renderer2D::StartBatch()
	{
		RendererData->Quad.ShapeBuffer.Begin();
		RendererData->Circle.ShapeBuffer.Begin();
		RendererData->Line.ShapeBuffer.Begin();

		CurrentFrameData.TextureSlotIndex = 1;
		
		// Clear texture slots
		for (int i = 1; i < MaxTextureSlots; i++)
			CurrentFrameData.TextureSlots[i] = nullptr;
	}

	void Renderer2D::Flush()
	{
		RendererData->Quad.ShapeBuffer.End();
		RendererData->Circle.ShapeBuffer.End();
		RendererData->Line.ShapeBuffer.End();
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const QuadDrawProperties& Props, int entityID)
	{
		DE_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		float textureIndex = 0.0f;
		float tilingFactor = 1.f;

		if (Props.TextureRef)
		{
			// Check if texture is already used
			for (uint32_t i = 1; i < CurrentFrameData.TextureSlotIndex; i++)
			{
				if (CurrentFrameData.TextureSlots[i] == Props.TextureRef)
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				if (CurrentFrameData.TextureSlotIndex >= MaxTextureSlots)
					NextBatch();

				textureIndex = (float)CurrentFrameData.TextureSlotIndex;
				CurrentFrameData.TextureSlots.at((size_t)textureIndex) = Props.TextureRef;
				CurrentFrameData.TextureSlotIndex++;
			}

			tilingFactor = Props.TilingFactor;
		}

		RendererData->Quad.ShapeBuffer.DrawShape(
		[&Props, textureIndex, tilingFactor, textureCoords, entityID](size_t idx)
		{
			QuadVertex val = {
				Props.Transform * QuadVertexPositions[idx],
				Props.Color,
				textureCoords[idx],
				textureIndex,
				tilingFactor,
				entityID
			};

			return val;
		});

		Stats.QuadCount++;
	}

	void Renderer2D::DrawCircle(const CircleDrawProperties& Props, int entityID)
	{
		DE_PROFILE_FUNCTION();

		RendererData->Circle.ShapeBuffer.DrawShape(
		[&Props, entityID](size_t idx)
		{
			CircleVertex val = {
				Props.Transform * QuadVertexPositions[idx],
				QuadVertexPositions[idx] * 2.0f,
				Props.Color,
				Props.Thickness,
				Props.Fade,
				entityID
			};

			return val;
		});

		Stats.QuadCount++;
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color)
	{
		RendererData->Line.ShapeBuffer.DrawVertex({ p0, color });

		RendererData->Line.ShapeBuffer.DrawVertex({ p1, color });
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

	void Renderer2D::SetLineWidth(float Width)
	{
		RendererData->Line.LineWidth = Width;
	}

	float Renderer2D::GetLineWidth()
	{
		return RendererData->Line.LineWidth;
	}

	void Renderer2D::ResetStats()
	{
		memset(&Stats, 0, sizeof(Statistics));
	}

	const Renderer2D::Statistics& Renderer2D::GetStats()
	{
		return Stats;
	}

	void Renderer2D::SetFlushCallbacks()
	{
		RendererData->Quad.ShapeBuffer.OnFlush(Renderer2D::FlushQuads);

		RendererData->Circle.ShapeBuffer.OnFlush(Renderer2D::FlushCircles);

		RendererData->Line.ShapeBuffer.OnFlush(Renderer2D::FlushLines);
	}

	void Renderer2D::FlushQuads()
	{
		DE_PROFILE_FUNCTION();

		// Bind textures
		for (uint32_t i = 0; i < CurrentFrameData.TextureSlotIndex; i++)
			CurrentFrameData.TextureSlots[i]->Bind(i);

		size_t IndexCount = RendererData->Quad.ShapeBuffer.GetShapeCount() * 6;

		Renderer::DrawIndexed(
			RendererData->Quad.ShaderRef,
			RendererData->Quad.ShapeBuffer.GetVertexArray(),
			(uint32_t)IndexCount
		);

		Stats.DrawCalls++;
	}

	void Renderer2D::FlushCircles()
	{
		size_t IndexCount = RendererData->Circle.ShapeBuffer.GetShapeCount() * 6;

		Renderer::DrawIndexed(
			RendererData->Circle.ShaderRef,
			RendererData->Circle.ShapeBuffer.GetVertexArray(),
			(uint32_t)IndexCount
		);

		Stats.DrawCalls++;
	}

	void Renderer2D::FlushLines()
	{
		size_t IndexCount = RendererData->Line.ShapeBuffer.GetShapeCount() * 2;

		Renderer::SetLineWidth(RendererData->Line.LineWidth);

		Renderer::DrawIndexed(
			RendererData->Line.ShaderRef,
			RendererData->Line.ShapeBuffer.GetVertexArray(),
			(uint32_t)IndexCount
		);

		Stats.DrawCalls++;
	}
}