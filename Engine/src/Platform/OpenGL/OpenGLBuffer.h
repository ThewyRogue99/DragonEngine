#pragma once

#include "Engine/Renderer/Buffer.h"

namespace Engine
{
	class ENGINE_API OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);

		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const BufferLayout& GetLayout() const override { return Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { Layout = layout; }

	private:
		uint32_t BufferID;

		BufferLayout Layout;
	};

	class ENGINE_API OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);

		virtual ~OpenGLIndexBuffer();

		virtual uint32_t GetCount() const override { return Count; }

		virtual void Bind() const override;
		virtual void Unbind() const override;

	private:
		uint32_t BufferID;
		uint32_t Count;
	};
}
