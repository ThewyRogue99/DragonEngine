#include "depch.h"
#include "OpenGLVertexArray.h"

#include "Engine/Debug/Debug.h"

#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Engine
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case Engine::ShaderDataType::Float:   return GL_FLOAT;
			case Engine::ShaderDataType::Float2:  return GL_FLOAT;
			case Engine::ShaderDataType::Float3:  return GL_FLOAT;
			case Engine::ShaderDataType::Float4:  return GL_FLOAT;
			case Engine::ShaderDataType::Mat3:    return GL_FLOAT;
			case Engine::ShaderDataType::Mat4:    return GL_FLOAT;
			case Engine::ShaderDataType::Int:     return GL_INT;
			case Engine::ShaderDataType::Int2:    return GL_INT;
			case Engine::ShaderDataType::Int3:    return GL_INT;
			case Engine::ShaderDataType::Int4:    return GL_INT;
			case Engine::ShaderDataType::Bool:    return GL_BOOL;
			default:
			{
				DE_ASSERT(false, "Unknown ShaderDataType!");
				return 0;
			}
		}
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &ArrayID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &ArrayID);
		ArrayID = 0;
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(ArrayID);

		// Bind Vertex Buffers
		for (auto& buff : VertexBuffers)
		{
			std::dynamic_pointer_cast<OpenGLVertexBuffer>(buff)->Bind();
		}

		// Bind Index Buffers
		if (indexBuffer)
		{
			std::dynamic_pointer_cast<OpenGLIndexBuffer>(indexBuffer)->Bind();
		}
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		DE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(ArrayID);

		std::dynamic_pointer_cast<OpenGLVertexBuffer>(vertexBuffer)->Bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				glEnableVertexAttribArray(index);

				glVertexAttribPointer(
					index,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)((uint8_t)element.Offset)
				);

				index++;
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(index);

				glVertexAttribIPointer(
					index,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					layout.GetStride(),
					(const void*)((uint8_t)element.Offset)
				);

				index++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint8_t count = element.GetComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(index);

					glVertexAttribPointer(
						index,
						count,
						ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)(element.Offset + sizeof(float) * count * i)
					);

					glVertexAttribDivisor(index, 1);

					index++;
				}
				break;
			}
			default:
				DE_ASSERT(false, "Unknown ShaderDataType!");
			}
		}

		VertexBuffers.push_back(vertexBuffer);

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		Bind();
		this->indexBuffer = indexBuffer;
		Unbind();
	}
}