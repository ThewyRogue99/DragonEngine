#pragma once

#include "Engine/Core/Core.h"

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

	struct ENGINE_API BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) { }

		uint32_t GetComponentCount() const;
	};

	class ENGINE_API BufferLayout
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
		void CalculateOffsetsAndStride();

		std::vector<BufferElement> Elements;
		uint32_t Stride = 0;
	};

	class ENGINE_API VertexBuffer
	{
	public:
		virtual ~VertexBuffer() { }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	class ENGINE_API IndexBuffer
	{
	public:
		virtual ~IndexBuffer() { }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t size);
	};
}