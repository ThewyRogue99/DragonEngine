#pragma once

#include "Engine/Renderer/VertexArray.h"

namespace Engine
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		ENGINE_API OpenGLVertexArray();
		ENGINE_API virtual ~OpenGLVertexArray();

		ENGINE_API virtual void Bind() const;
		ENGINE_API virtual void Unbind() const;

		ENGINE_API virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		ENGINE_API virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		inline virtual std::vector<Ref<VertexBuffer>> GetVertexBuffers() const override { return VertexBuffers; }
		inline virtual Ref<IndexBuffer> GetIndexBuffer() const override { return indexBuffer; }

	private:
		uint32_t ArrayID;

		std::vector<Ref<VertexBuffer>> VertexBuffers;
		Ref<IndexBuffer> indexBuffer;
	};
}
