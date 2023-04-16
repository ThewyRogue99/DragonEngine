#pragma once

#include "Engine/Core/Core.h"

#include "Buffer.h"

#include <vector>

namespace Engine
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() { }

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		virtual std::vector<Ref<VertexBuffer>> GetVertexBuffers() const = 0;
		virtual Ref<IndexBuffer> GetIndexBuffer() const = 0;

		ENGINE_API static Ref<VertexArray> Create();
	};
}