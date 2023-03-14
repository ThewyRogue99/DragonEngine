#pragma once

#include "Engine/Core/Core.h"

#include "VertexArray.h"
#include "Buffer.h"

#include <functional>

namespace Engine
{
	template<typename T>
	class BatchShapeBuffer
	{
	public:
		BatchShapeBuffer(size_t ShapeCount, size_t ShapeVertexCount)
			: ShapeCount(ShapeCount), ShapeVertexCount(ShapeVertexCount) { }

		void CreateBuffer(const BufferLayout& Layout)
		{
			VertexArrayRef = VertexArray::Create();

			size_t VertexCount = GetMaxBufferVertexCount();
			VertexBufferRef = VertexBuffer::Create((uint32_t)(VertexCount * sizeof(T)));
			VertexBufferRef->SetLayout(Layout);

			VertexArrayRef->AddVertexBuffer(VertexBufferRef);

			BufferBase = new T[VertexCount];
		}

		Ref<VertexArray> GetVertexArray() const { return VertexArrayRef; }
		Ref<VertexBuffer> GetVertexBuffer() const { return VertexBufferRef; }

		void Begin()
		{
			StartBatch();
		}

		void DrawVertex(const T& Vertex)
		{
			if (GetAvailableVertexCount() < 1)
				NextBatch();

			*BufferPtr = Vertex;
			BufferPtr++;
			VertexIndex++;
		}

		void DrawShape(std::function<T(size_t)> Callback)
		{
			if (Callback)
			{
				if (GetAvailableVertexCount() < ShapeVertexCount)
					NextBatch();

				for (size_t i = 0; i < ShapeVertexCount; i++)
				{
					*BufferPtr = Callback(i);
					BufferPtr++;
					VertexIndex++;
				}
			}
		}

		void End()
		{
			Flush();
		}

		const size_t GetShapeCount() const { return VertexIndex / ShapeVertexCount; }

		void OnFlush(std::function<void()> Callback) { OnFlushCallback = Callback; }

	private:
		void StartBatch()
		{
			VertexIndex = 0;
			BufferPtr = BufferBase;
		}

		void NextBatch()
		{
			Flush();
			StartBatch();
		}

		void Flush()
		{
			if (VertexIndex > 0)
			{
				uint32_t dataSize = (uint32_t)((uint8_t*)BufferPtr - (uint8_t*)BufferBase);
				VertexBufferRef->SetData(BufferBase, dataSize);

				if (OnFlushCallback)
				{
					OnFlushCallback();
				}
			}
		}

		const size_t GetMaxBufferVertexCount() const { return ShapeCount * ShapeVertexCount; }

		const size_t GetAvailableVertexCount() const { return GetMaxBufferVertexCount() - VertexIndex; }
		const size_t GetAvailableShapeCount() const { return GetAvailableVertexCount() / ShapeVertexCount; }

	private:
		size_t ShapeCount;
		size_t ShapeVertexCount;

		size_t VertexIndex = 0;

		T* BufferBase = nullptr;
		T* BufferPtr = nullptr;

		std::function<void()> OnFlushCallback = nullptr;

		Ref<VertexArray> VertexArrayRef = nullptr;
		Ref<VertexBuffer> VertexBufferRef = nullptr;
	};

}