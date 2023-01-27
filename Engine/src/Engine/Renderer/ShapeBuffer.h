#pragma once

#include "Engine/Core/Core.h"

#include "Shader.h"
#include "VertexArray.h"
#include "Buffer.h"

#include <functional>

namespace Engine
{
	template<typename T>
	class ShapeBuffer
	{
	public:
		ShapeBuffer(size_t ShapeCount, size_t ShapeVertexCount)
			: ShapeCount(ShapeCount), ShapeVertexCount(ShapeVertexCount) { }

		void CreateBuffer(const BufferLayout& Layout)
		{
			m_VertexArray = VertexArray::Create();

			size_t VertexCount = GetMaxBufferVertexCount();
			m_VertexBuffer = VertexBuffer::Create((uint32_t)(VertexCount * sizeof(T)));
			m_VertexBuffer->SetLayout(Layout);

			m_VertexArray->AddVertexBuffer(m_VertexBuffer);

			BufferBase = new T[VertexCount];
		}

		void SetShader(Ref<Shader> NewShader)
		{
			m_Shader = NewShader;
		}

		Ref<Shader> GetShader() const { return m_Shader; }
		Ref<VertexArray> GetVertexArray() const { return m_VertexArray; }
		Ref<VertexBuffer> GetVertexBuffer() const { return m_VertexBuffer; }

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
				m_VertexBuffer->SetData(BufferBase, dataSize);

				if (m_Shader)
					m_Shader->Bind();

				OnFlushCallback();
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

		Ref<Shader> m_Shader = nullptr;
		Ref<VertexArray> m_VertexArray = nullptr;
		Ref<VertexBuffer> m_VertexBuffer = nullptr;
	};

}