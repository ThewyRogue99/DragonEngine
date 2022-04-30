#include "depch.h"
#include "OpenGLVertexArray.h"

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
				DE_CORE_ASSERT(false, "Unknown ShaderDataType!");
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
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		DE_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(ArrayID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset
			);
			index++;
		}

		VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(ArrayID);
		indexBuffer->Bind();

		this->indexBuffer = indexBuffer;
	}
}