#pragma once
#include "depch.h"

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

namespace Engine
{
	enum class ShaderDataType : uint8_t
	{
		None,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Bool
	};

	uint32_t ShaderDataTypeSize(ShaderDataType type);

	class BufferElement
	{
	public:
		BufferElement(ShaderDataType type, const WString& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) { }

		WString Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		ENGINE_API uint32_t GetComponentCount() const;
	};

	class BufferLayout
	{
	public:
		BufferLayout() { }

		BufferLayout(const std::initializer_list<BufferElement>& layout) : Elements(layout)
		{
			CalculateOffsetsAndStride();
		}

		inline const std::vector<BufferElement>& GetElements() const { return Elements; }
		inline uint32_t GetStride() const { return Stride; }

		inline std::vector<BufferElement>::iterator begin() { return Elements.begin(); }
		inline std::vector<BufferElement>::iterator end() { return Elements.end(); }

		inline std::vector<BufferElement>::const_iterator begin() const { return Elements.begin(); }
		inline std::vector<BufferElement>::const_iterator end() const { return Elements.end(); }

	private:
		ENGINE_API void CalculateOffsetsAndStride();

		std::vector<BufferElement> Elements = { };
		uint32_t Stride = 0;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() { }

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		ENGINE_API static Ref<VertexBuffer> Create(uint32_t size);
		ENGINE_API static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() { }

		virtual uint32_t GetCount() const = 0;

		ENGINE_API static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};
}