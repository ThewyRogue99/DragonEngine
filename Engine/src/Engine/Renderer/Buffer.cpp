#include "depch.h"
#include "Buffer.h"

#include "Engine/Debug/Assert.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Engine
{
	uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:
				return 4;
			case ShaderDataType::Float2:
				return 4 * 2;
			case ShaderDataType::Float3:
				return 4 * 3;
			case ShaderDataType::Float4:
				return 4 * 4;
			case ShaderDataType::Mat3:
				return 4 * 3 * 3;
			case ShaderDataType::Mat4:
				return 4 * 4 * 4;
			case ShaderDataType::Int:
				return 4;
			case ShaderDataType::Int2:
				return 4 * 2;
			case ShaderDataType::Int3:
				return 4 * 3;
			case ShaderDataType::Int4:
				return 4 * 4;
			case ShaderDataType::Bool:
				return 1;
			default:
			{
				DE_ASSERT(false, "Unknown ShaderDataType!");
				return 0;
			}
		}
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:
			{
				DE_ASSERT(false, "No renderer API is used for renderer!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				return CreateRef<OpenGLVertexBuffer>(size);
			}
			default:
			{
				DE_ASSERT(false, "Unknown Renderer API!");
				return nullptr;
			}
		}
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:
			{
				DE_ASSERT(false, "No renderer API is used for renderer!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				return std::make_shared<OpenGLVertexBuffer>(vertices, size);
			}
			default:
			{
				DE_ASSERT(false, "Unknown Renderer API!");
				return nullptr;
			}
		}
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:
			{
				DE_ASSERT(false, "No renderer API is used for renderer!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				return std::make_shared<OpenGLIndexBuffer>(indices, count);
			}
			default:
			{
				DE_ASSERT(false, "Unknown Renderer API!");
				return nullptr;
			}
		}
	}

	void BufferLayout::CalculateOffsetsAndStride()
	{
		uint32_t offset = 0;
		Stride = 0;

		for (auto& element : Elements)
		{
			element.Offset = offset;
			offset += element.Size;
			Stride += element.Size;
		}
	}
	uint32_t BufferElement::GetComponentCount() const
	{
		switch (Type)
		{
			case ShaderDataType::Float:
				return 1;
			case ShaderDataType::Float2:
				return 2;
			case ShaderDataType::Float3:
				return 3;
			case ShaderDataType::Float4:
				return 4;
			case ShaderDataType::Mat3:
				return 3 * 3;
			case ShaderDataType::Mat4:
				return 4 * 4;
			case ShaderDataType::Int:
				return 1;
			case ShaderDataType::Int2:
				return 2;
			case ShaderDataType::Int3:
				return 3;
			case ShaderDataType::Int4:
				return 4;
			case ShaderDataType::Bool:
				return 1;
			default:
			{
				DE_ASSERT(false, "Unknown ShaderDataType!");
				return 0;
			}
		}
	}
}