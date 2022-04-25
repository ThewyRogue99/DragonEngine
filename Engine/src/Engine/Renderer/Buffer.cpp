#include "depch.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

#include "Renderer.h"

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
				DE_CORE_ASSERT(false, "Unknown ShaderDataType!");
				return 0;
			}
		}
	}

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
			{
				DE_CORE_ASSERT(false, "No renderer API is used for renderer!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				return new OpenGLVertexBuffer(vertices, size);
			}
			default:
			{
				DE_CORE_ASSERT(false, "Unknown Renderer API!");
				return nullptr;
			}
		}
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
			{
				DE_CORE_ASSERT(false, "No renderer API is used for renderer!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				return new OpenGLIndexBuffer(indices, size);
			}
			default:
			{
				DE_CORE_ASSERT(false, "Unknown Renderer API!");
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
				DE_CORE_ASSERT(false, "Unknown ShaderDataType!");
				return 0;
			}
		}
	}

}