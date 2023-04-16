#pragma once

#include "Engine/Renderer/Buffer.h"

namespace Engine
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		ENGINE_API OpenGLVertexBuffer(uint32_t size);
		ENGINE_API OpenGLVertexBuffer(float* vertices, uint32_t size);

		ENGINE_API virtual ~OpenGLVertexBuffer();

		ENGINE_API virtual void Bind() const;
		ENGINE_API virtual void Unbind() const;

		virtual const BufferLayout& GetLayout() const override { return Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { Layout = layout; }

		ENGINE_API virtual void SetData(const void* data, uint32_t size) override;

	private:
		uint32_t BufferID;

		BufferLayout Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		ENGINE_API OpenGLIndexBuffer(uint32_t* indices, uint32_t count);

		ENGINE_API virtual ~OpenGLIndexBuffer();

		virtual uint32_t GetCount() const override { return Count; }

		ENGINE_API virtual void Bind() const;
		ENGINE_API virtual void Unbind() const;

	private:
		uint32_t BufferID;
		uint32_t Count;
	};
}
