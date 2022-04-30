#pragma once

#include "Engine/Renderer/VertexArray.h"

namespace Engine
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		inline virtual std::vector<Ref<VertexBuffer>> GetVertexBuffers() const override { return VertexBuffers; }
		inline virtual Ref<IndexBuffer> GetIndexBuffer() const override { return indexBuffer; }

	private:
		uint32_t ArrayID;

		std::vector<Ref<VertexBuffer>> VertexBuffers;
		Ref<IndexBuffer> indexBuffer;
	};
}
