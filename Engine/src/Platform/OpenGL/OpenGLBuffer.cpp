#include "depch.h"
#include "OpenGLBuffer.h"

#include "Engine/Debug/Instrumentor.h"

#include <glad/glad.h>

namespace Engine
{
	// ------------------------ Vertex Buffer ----------------------------

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		DE_PROFILE_FUNCTION();

		glCreateBuffers(1, &BufferID);

		Bind();
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		Unbind();
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		DE_PROFILE_FUNCTION();

		glCreateBuffers(1, &BufferID);

		Bind();
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		Unbind();
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		DE_PROFILE_FUNCTION();

		glDeleteBuffers(1, &BufferID);
		BufferID = 0;
	}

	void OpenGLVertexBuffer::Bind() const
	{
		DE_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, BufferID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		DE_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		Bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
		Unbind();
	}

	// ------------------------ Index Buffer ----------------------------

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : Count(count)
	{
		glCreateBuffers(1, &BufferID);

		Bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
		Unbind();
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &BufferID);
		BufferID = 0;
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}