#include "depch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Engine
{
	// ------------------------ Vertex Buffer ----------------------------

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		DE_PROFILE_FUNCTION();

		glCreateBuffers(1, &BufferID);

		glBindBuffer(GL_ARRAY_BUFFER, BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		DE_PROFILE_FUNCTION();

		glCreateBuffers(1, &BufferID);

		glBindBuffer(GL_ARRAY_BUFFER, BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
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
		glBindBuffer(GL_ARRAY_BUFFER, BufferID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	// ------------------------ Index Buffer ----------------------------

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : Count(count)
	{
		glCreateBuffers(1, &BufferID);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
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