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

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		inline virtual std::vector<std::shared_ptr<VertexBuffer>> GetVertexBuffers() const override { return VertexBuffers; }
		inline virtual std::shared_ptr<IndexBuffer> GetIndexBuffer() const override { return indexBuffer; }

	private:
		uint32_t ArrayID;

		std::vector<std::shared_ptr<VertexBuffer>> VertexBuffers;
		std::shared_ptr<IndexBuffer> indexBuffer;
	};
}
